#ifndef _CONVERTER_H_
#define _CONVERTER_H_

#ifdef __cplusplus
#define _in(T) const T &
#define _inout(T) T &
#define _out(T) T &
#define _begin(type) type {
#define _end }
#define _mutable(T) T
#define _constant(T) const T
#define mul(a, b) (a) * (b)
#endif

#if defined(GL_ES) || defined(GL_SHADING_LANGUAGE_VERSION)
#define _in(T) const in T
#define _inout(T) inout T
#define _out(T) out T
#define _begin(type) type (
#define _end )
#define _mutable(T) T
#define _constant(T) const T
#define mul(a, b) (a) * (b)
precision mediump float;
#endif

#ifdef HLSL
#define _in(T) const in T
#define _inout(T) inout T
#define _out(T) out T
#define _begin(type) {
#define _end }
#define _mutable(T) static T
#define _constant(T) static const T
#define vec2 float2
#define vec3 float3
#define vec4 float4
#define mat2 float2x2
#define mat3 float3x3
#define mat4 float4x4
#define mix lerp
#define fract frac
#define atan(y, x) atan2(x, y)
#define mod fmod
#pragma pack_matrix(row_major)

#endif 