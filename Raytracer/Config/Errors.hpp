///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Error Message Definitions for Raytracer
///////////////////////////////////////////////////////////////////////////////

#define RAY_UNEXPECTED "Something Unexpected Append: "

///////////////////////////////////////////////////////////////////////////////
// General Errors
///////////////////////////////////////////////////////////////////////////////

#define RAY_ERROR_GENERAL "Error: "
#define RAY_ERROR_NOT_IMPLEMENTED "Error: Feature not implemented"
#define RAY_ERROR_OUT_OF_MEMORY "Error: Out of memory"
#define RAY_ERROR_FILE_IO "Error: File I/O operation failed: "
#define RAY_ERROR_INVALID_PARAMETER "Error: Invalid parameter: "
#define RAY_ERROR_NULL_POINTER "Error: Null pointer encountered"
#define RAY_ERROR_DIVIDE_BY_ZERO "Error: Division by zero"
#define RAY_ERROR_OVERFLOW "Error: Numerical overflow detected"
#define RAY_ERROR_IMGUI "Error: Failed to initialize ImGui"

///////////////////////////////////////////////////////////////////////////////
// Scene Loading Errors
///////////////////////////////////////////////////////////////////////////////

#define RAY_ERROR_SCENE_LOAD "Error: Failed to load scene: "
#define RAY_ERROR_MISSING_TEXTURE "Error: Texture not found: "
#define RAY_ERROR_FAILED_TEXTURE "Error: Failed to load texture"
#define RAY_ERROR_MISSING_MODEL "Error: Model not found: "
#define RAY_ERROR_MISSING_SHADER "Error: Shader not found: "
#define RAY_ERROR_FAILED_OPEN_SHADER "Failed to open shader file: "
#define RAY_ERROR_SCENE_PARSE "Error: Failed to parse scene file: "
#define RAY_ERROR_INVALID_GEOMETRY "Error: Invalid geometry definition: "

///////////////////////////////////////////////////////////////////////////////
// Rendering Errors
///////////////////////////////////////////////////////////////////////////////

#define RAY_ERROR_RENDER_FAIL "Error: Rendering failed: "
#define RAY_ERROR_MAX_DEPTH "Error: Maximum recursion depth exceeded"
#define RAY_ERROR_NO_CAMERA "Error: No camera defined in scene"
#define RAY_ERROR_INVALID_BVH "Error: Invalid BVH structure"
#define RAY_ERROR_NO_ACCELERATION "Error: No acceleration structure built"
#define RAY_ERROR_SHADER_COMPILATION "Error: Shader compilation failed: "

///////////////////////////////////////////////////////////////////////////////
// Material System Errors
///////////////////////////////////////////////////////////////////////////////

#define RAY_ERROR_MATERIAL_INVALID "Error: Invalid material definition: "
#define RAY_ERROR_UNKNOWN_BSDF "Error: Unknown BSDF type: "
#define RAY_ERROR_MATERIAL_PARAM "Error: Invalid material parameter: "

///////////////////////////////////////////////////////////////////////////////
// Threading/Parallelism Errors
///////////////////////////////////////////////////////////////////////////////

#define RAY_ERROR_THREAD_CREATE "Error: Failed to create thread"
#define RAY_ERROR_THREAD_JOIN "Error: Failed to join thread"
#define RAY_ERROR_MUTEX_LOCK "Error: Failed to lock mutex"
#define RAY_ERROR_RACE_CONDITION "Error: Race condition detected"
