#pragma once

#include <d3d12.h>
#include <string>

#include <d3dcompiler.h>
#include <dxcapi.h>

class Shader {
public:
	void Load(const std::wstring& filePath, const std::wstring& shaderModel);
	void LoadDxc(const std::wstring& filePath, const std::wstring& shaderModel);

	ID3DBlob* GetBlob();
	IDxcBlob* GetDxcBlob();

	Shader();

	~Shader();

private:
	ID3DBlob* blob_ = nullptr;

	IDxcBlob* dxcBlob_ = nullptr;
};
