#pragma once

#include "IAsset.h"

class MetaFileData;
class MeshFileData;
class BoneFileData;
class PrefabFileData;
class ShaderFileData;
class TextureFileData;
class PhysxMaterialFileData;
class SoundFileData;
class MovieFileData;
class AnimationFileData;

using MetaAsset = Asset<MetaFileData>;
using MeshAsset = Asset<MeshFileData>;
using BoneAsset = Asset<BoneFileData>;
using PrefabAsset = Asset<PrefabFileData>;
using ShaderAsset = Asset<ShaderFileData>;
using TextureAsset = Asset<TextureFileData>;
using PhysxMaterialAsset = Asset<PhysxMaterialFileData>;
using SoundAsset = Asset<SoundFileData>;
using MovieAsset = Asset<MovieFileData>;
using AnimationAsset = Asset<AnimationFileData>;