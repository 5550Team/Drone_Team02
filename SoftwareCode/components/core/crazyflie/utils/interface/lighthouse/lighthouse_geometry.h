#pragma once

#include <stdbool.h>
#include "xtensa_math.h"
#include "stabilizer_types.h"

typedef struct {
  __attribute__((aligned(4))) vec3d origin;
  __attribute__((aligned(4))) mat3d mat;
  bool valid;
} __attribute__((packed)) baseStationGeometry_t;

typedef struct {
  __attribute__((aligned(4))) mat3d baseStationInvertedRotationMatrixes;
  __attribute__((aligned(4))) mat3d lh1Rotor2RotationMatrixes;
  __attribute__((aligned(4))) mat3d lh1Rotor2InvertedRotationMatrixes;
} baseStationGeometryCache_t;


bool lighthouseGeometryGetPositionFromRayIntersection(const baseStationGeometry_t baseStations[2], float angles1[2], float angles2[2], vec3d position, float *position_delta);

void lighthouseGeometryGetBaseStationPosition(const baseStationGeometry_t* baseStationGeometry, vec3d baseStationPos);

void lighthouseGeometryGetRay(const baseStationGeometry_t* baseStationGeometry, const float angle1, const float angle2, vec3d ray);

bool lighthouseGeometryIntersectionPlaneVector(const vec3d linePoint, const vec3d lineVec, const vec3d planePoint, const vec3d PlaneNormal, vec3d intersectionPoint);

void lighthouseGeometryGetSensorPosition(const vec3d cfPos, const arm_matrix_instance_f32 *R, vec3d sensorPosition, vec3d pos);

bool lighthouseGeometryYawDelta(const vec3d ipv, const vec3d spv, const vec3d n, float* yawDelta);
