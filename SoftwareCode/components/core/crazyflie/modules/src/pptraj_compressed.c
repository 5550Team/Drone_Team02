#include <stdint.h>
#include <string.h>

#include "pptraj_compressed.h"

// In the stored version, we store durations in milliseconds, hence this factor
#define STORED_DURATION_SCALE 1000.0

// In the stored version, we store distances in millimeters, hence this factor
#define STORED_DISTANCE_SCALE 1000.0

// In the stored version, we store angles in 1/10th degrees, hence this factor
#define STORED_ANGLE_SCALE 0.1

static const uint8_t control_points_by_type[] = { 0, 1, 3, 7 };

typedef int16_t compressed_piece_coordinate;
typedef const uint8_t* compressed_piece_ptr;

struct compressed_piece_parsed_header
{
  uint16_t duration_in_msec;

  enum piecewise_traj_storage_type x_type;
  enum piecewise_traj_storage_type y_type;
  enum piecewise_traj_storage_type z_type;
  enum piecewise_traj_storage_type yaw_type;

  compressed_piece_ptr body;
};

static float calculate_total_duration(compressed_piece_ptr ptr);
static compressed_piece_ptr next_coordinate(compressed_piece_ptr ptr, compressed_piece_coordinate* coord);
static compressed_piece_ptr next_duration(compressed_piece_ptr ptr, uint16_t* coord);
static compressed_piece_ptr next_piece(compressed_piece_ptr ptr);
static compressed_piece_ptr parse_header_of_current_piece(
  struct compressed_piece_parsed_header* result, compressed_piece_ptr ptr);

static inline float end_time_of_current_piece(const struct piecewise_traj_compressed *traj);
static inline float start_time_of_current_piece(const struct piecewise_traj_compressed *traj);
static inline float time_relative_to_start_of_current_piece(const struct piecewise_traj_compressed *traj, float t);

static void piecewise_compressed_advance_playhead(struct piecewise_traj_compressed *traj);
static void piecewise_compressed_rewind(struct piecewise_traj_compressed *traj);
static void piecewise_compressed_update_current_poly4d(
  struct piecewise_traj_compressed *traj, const struct traj_eval *end_of_previous_piece);


static compressed_piece_ptr calculate_polynomial_coefficients(
  float *result, compressed_piece_ptr ptr,
  enum piecewise_traj_storage_type storage_type,
  float initial_condition, float duration, float scale)
{
  compressed_piece_coordinate value;
  float control_points[PP_SIZE];
  uint8_t i, n;

  n = control_points_by_type[storage_type] + 1;

  control_points[0] = initial_condition;
  for (i = 1; i < n; i++) {
    ptr = next_coordinate(ptr, &value);
    control_points[i] = value / scale;
  }

  polybezier(result, duration, control_points, n);

  return ptr;
}

// Calculates the total duration of a compressed trajectory, starting at the
// given piece
static float calculate_total_duration(compressed_piece_ptr ptr)
{
  uint32_t duration_in_msec = 0;
  struct compressed_piece_parsed_header header;

  while (1) {
    ptr = parse_header_of_current_piece(&header, ptr);
    if (ptr) {
      duration_in_msec += header.duration_in_msec;
    } else {
      return duration_in_msec / STORED_DURATION_SCALE;
    }
  }
}

// Returns the end time of the current piece being executed
static inline float end_time_of_current_piece(const struct piecewise_traj_compressed *traj) {
  return start_time_of_current_piece(traj) + traj->current_piece.poly4d.duration;
}


static compressed_piece_ptr next_coordinate(compressed_piece_ptr ptr, compressed_piece_coordinate* coord) {
  *coord = ptr[0] + (ptr[1] << 8);
  return ptr + 2;
}


static compressed_piece_ptr next_duration(compressed_piece_ptr ptr, uint16_t* coord) {
  *coord = ptr[0] + (ptr[1] << 8);
  return ptr + 2;
}


static compressed_piece_ptr next_piece(compressed_piece_ptr ptr)
{
  struct compressed_piece_parsed_header header;
  return parse_header_of_current_piece(&header, ptr);
}

static compressed_piece_ptr parse_header_of_current_piece(
  struct compressed_piece_parsed_header* result, compressed_piece_ptr ptr)
{
  uint8_t header;
  int length;

  if (!ptr) {
    result->x_type = result->y_type = result->z_type = result->yaw_type = PPTRAJ_STORAGE_CONSTANT;
    result->body = 0;
    result->duration_in_msec = 0;
    return 0;
  }

  header = *ptr;

  result->x_type   = (enum piecewise_traj_storage_type) (header & 0x03);
  result->y_type   = (enum piecewise_traj_storage_type) ((header >> 2) & 0x03);
  result->z_type   = (enum piecewise_traj_storage_type) ((header >> 4) & 0x03);
  result->yaw_type = (enum piecewise_traj_storage_type) ((header >> 6) & 0x03);
  ptr++;

  result->body = ptr = next_duration(ptr, &result->duration_in_msec);

  if (result->duration_in_msec > 0) {
    length = (
      control_points_by_type[result->x_type] +
      control_points_by_type[result->y_type] +
      control_points_by_type[result->z_type] +
      control_points_by_type[result->yaw_type]
    ) * sizeof(compressed_piece_coordinate);
    return ptr + length;
  } else {
    return 0;
  }
}

// Returns the start time of the current piece being executed
static inline float start_time_of_current_piece(const struct piecewise_traj_compressed *traj) {
  return traj->t_begin + traj->current_piece.t_begin_relative;
}

// Returns the number of seconds elapsed since the start time of the current
// piece being executed
static inline float time_relative_to_start_of_current_piece(const struct piecewise_traj_compressed *traj, float t) {
  return t - start_time_of_current_piece(traj);
}


struct traj_eval piecewise_compressed_eval(
  struct piecewise_traj_compressed *traj, float t)
{
  struct traj_eval eval;


  if (t < start_time_of_current_piece(traj)) {
    piecewise_compressed_rewind(traj);
  }

  while (traj->current_piece.data && t >= end_time_of_current_piece(traj)) {
    piecewise_compressed_advance_playhead(traj);
  }

  t = time_relative_to_start_of_current_piece(traj, t);

  eval = poly4d_eval(&traj->current_piece.poly4d, t);
  eval.pos = vadd(eval.pos, traj->shift);

  return eval;
}

void piecewise_compressed_load(struct piecewise_traj_compressed *traj, const void* data)
{
  traj->t_begin = 0;
  traj->timescale = 1;

  traj->data = data;
  traj->shift = vzero();
  piecewise_compressed_rewind(traj);

  traj->duration = calculate_total_duration(traj->current_piece.data);
}

static void piecewise_compressed_rewind(struct piecewise_traj_compressed *traj)
{
  struct traj_eval stopped;
  compressed_piece_coordinate value;
  compressed_piece_ptr ptr;

  /* Parse header that stores the start coordinates */
  bzero(&stopped, sizeof(stopped));
  ptr = traj->data;
  ptr = next_coordinate(ptr, &value); stopped.pos.x = value / STORED_DISTANCE_SCALE;
  ptr = next_coordinate(ptr, &value); stopped.pos.y = value / STORED_DISTANCE_SCALE;
  ptr = next_coordinate(ptr, &value); stopped.pos.z = value / STORED_DISTANCE_SCALE;
  ptr = next_coordinate(ptr, &value); stopped.yaw = value / STORED_ANGLE_SCALE;
  traj->current_piece.t_begin_relative = 0;
  traj->current_piece.data = ptr;

  piecewise_compressed_update_current_poly4d(traj, &stopped);
}

static void piecewise_compressed_update_current_poly4d(
  struct piecewise_traj_compressed *traj, const struct traj_eval *prev_end)
{
  struct poly4d* poly4d = &traj->current_piece.poly4d;
  compressed_piece_ptr ptr;
  struct compressed_piece_parsed_header header;

  /* First, clear everything in the poly4d */
  bzero(poly4d, sizeof(*poly4d));

  /* Parse the header of the current piece, extract the storage types and the duration */
  ptr = traj->current_piece.data;
  parse_header_of_current_piece(&header, ptr);
  poly4d->duration = header.duration_in_msec / STORED_DURATION_SCALE;

  /* Process the body */
  ptr = header.body;
  ptr = calculate_polynomial_coefficients(
    poly4d->p[0], ptr, header.x_type, prev_end->pos.x, poly4d->duration, STORED_DISTANCE_SCALE);
  ptr = calculate_polynomial_coefficients(
    poly4d->p[1], ptr, header.y_type, prev_end->pos.y, poly4d->duration, STORED_DISTANCE_SCALE);
  ptr = calculate_polynomial_coefficients(
    poly4d->p[2], ptr, header.z_type, prev_end->pos.z, poly4d->duration, STORED_DISTANCE_SCALE);
  calculate_polynomial_coefficients(
    poly4d->p[3], ptr, header.yaw_type, prev_end->yaw, poly4d->duration, STORED_ANGLE_SCALE);
}

static void piecewise_compressed_advance_playhead(struct piecewise_traj_compressed *traj)
{
  float duration = traj->current_piece.poly4d.duration;
  struct traj_eval end_of_previous_piece = poly4d_eval(&traj->current_piece.poly4d, duration);

  traj->current_piece.t_begin_relative += duration;
  traj->current_piece.data = next_piece(traj->current_piece.data);

  piecewise_compressed_update_current_poly4d(traj, &end_of_previous_piece);
}
