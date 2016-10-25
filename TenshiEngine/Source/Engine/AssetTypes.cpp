#include "AssetTypes.h"

const AssetFileType AssetDataTemplate<MetaFileData>::_AssetFileType = AssetFileType::Meta;
void AssetDataTemplate<MetaFileData>::CreateInspector() {}

const AssetFileType AssetDataTemplate<MeshFileData>::_AssetFileType = AssetFileType::Temesh;
void AssetDataTemplate<MeshFileData>::CreateInspector();

const AssetFileType AssetDataTemplate<BoneFileData>::_AssetFileType = AssetFileType::Tebone;
void AssetDataTemplate<BoneFileData>::CreateInspector();

const AssetFileType AssetDataTemplate<PrefabFileData>::_AssetFileType = AssetFileType::Prefab;
void AssetDataTemplate<PrefabFileData>::CreateInspector();

const AssetFileType AssetDataTemplate<ShaderFileData>::_AssetFileType = AssetFileType::Shader;
void AssetDataTemplate<ShaderFileData>::CreateInspector();

const AssetFileType AssetDataTemplate<TextureFileData>::_AssetFileType = AssetFileType::Image;
void AssetDataTemplate<TextureFileData>::CreateInspector() {}

const AssetFileType AssetDataTemplate<PhysxMaterialFileData>::_AssetFileType = AssetFileType::PhysxMaterial;
void AssetDataTemplate<PhysxMaterialFileData>::CreateInspector();

const AssetFileType AssetDataTemplate<SoundFileData>::_AssetFileType = AssetFileType::Sound;
void AssetDataTemplate<SoundFileData>::CreateInspector() {}

const AssetFileType AssetDataTemplate<MovieFileData>::_AssetFileType = AssetFileType::Movie;
void AssetDataTemplate<MovieFileData>::CreateInspector() {}

const AssetFileType AssetDataTemplate<AnimationFileData>::_AssetFileType = AssetFileType::Animation;
void AssetDataTemplate<AnimationFileData>::CreateInspector() {}