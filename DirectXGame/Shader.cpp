#include "Shader.h"
#include "MiscUtility.h"
#include <cassert>
#include <d3dcompiler.h>
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")

void Shader::Load(const std::wstring& filePath, const std::wstring& shaderModel) {
	ID3DBlob* ShaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	std::string mbShaderModel = ConvertString(shaderModel.c_str());

	HRESULT hr =
	    D3DCompileFromFile(filePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", mbShaderModel.c_str(), D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &ShaderBlob, &errorBlob);
	if (FAILED(hr)) {
		if (errorBlob) {
			OutputDebugStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		}
		assert(false);
	}

	blob_ = ShaderBlob;
}

void Shader::LoadDxc(const std::wstring& filePath, const std::wstring& shaderModel) {
	static IDxcUtils* dxcUtils = nullptr;
	static IDxcCompiler3* dxcCompiler = nullptr;
	static IDxcIncludeHandler* includeHandler = nullptr;

	HRESULT hr;

	if (dxcUtils == nullptr) {
		hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
		assert(SUCCEEDED(hr));
	}
	if (dxcCompiler == nullptr) {
		hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
		assert(SUCCEEDED(hr));
	}
	if (dxcUtils == nullptr) {
		hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
		assert(SUCCEEDED(hr));
	}

	IDxcBlobEncoding* shaderSource = nullptr;
	hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	assert(SUCCEEDED(hr));

	DxcBuffer shaderSourceBuffer{};
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	LPCWSTR arguments[] = {
	    filePath.c_str(), L"-E",  L"main", L"-T", shaderModel.c_str(), L"-Zi",
	    L"-Qembed_debug", L"-Od", L"-Zpr"

	};

	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
	    &shaderSourceBuffer, arguments, _countof(arguments), includeHandler, IID_PPV_ARGS(&shaderResult)

	);
	assert(SUCCEEDED(hr));

	IDxcBlobUtf8* shaderError = nullptr;
	IDxcBlobWide* nameBlob = nullptr;

	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), &nameBlob);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		OutputDebugStringA(shaderError->GetStringPointer());
		assert(false);
	}

	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), &nameBlob);
	assert(SUCCEEDED(hr));

	shaderSource->Release();
	shaderResult->Release();

	dxcBlob_ = shaderBlob;
}

ID3DBlob* Shader::GetBlob() { return blob_; }

IDxcBlob* Shader::GetDxcBlob() { return dxcBlob_; }

Shader::Shader() {}

Shader::~Shader() {
	if (blob_ != nullptr) {
		blob_->Release();
		blob_ = nullptr;
	}
	if (dxcBlob_ != nullptr) {
		dxcBlob_->Release();
		dxcBlob_ = nullptr;
	}
}
