/*
 * Copyright (c) 2020, NVIDIA CORPORATION. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __TRT_DCNV2_PLUGIN_H__
#define __TRT_DCNV2_PLUGIN_H__

#include "NvInferPlugin.h"
#include "NvInferRuntime.h"
#include <cuda.h>
#include <cudnn.h>
#include <cuda_runtime.h>
#include <cublas_v2.h>
#include <string>
#include <vector>
#include <memory>
#include "macros.h"
#include <iostream>
#include <stdio.h>

namespace nvinfer1
{

	struct DCNv2Parameters
	{
		std::vector<int> dilation;
		std::vector<int> padding;
		std::vector<int> stride;
		int deformable_groups;
	};

	const int DILATION_DIM = 2;
	const int PADDING_DIM = 2;
	const int STRIDE_DIM = 2;
	const int NUM_DCN_CHANNELS = 2;

	constexpr int N_DIM = 0;
	constexpr int C_DIM = 1;
	constexpr int H_DIM = 2;
	constexpr int W_DIM = 3;

	void enqueue_call(const void* const* inputs, void* const* outputs, void* workspace,
		cudaStream_t stream, const DataType& iType, const Dims& input1_shape,
		const Dims& input2_shape, const Dims& weights_shape, const Dims& output_shape,
		DataType mType, cublasHandle_t cublasHandle_, const DCNv2Parameters& mParam);

	inline unsigned int getElementSize(DataType t);

	class DCNv2PluginDynamic : public IPluginV2DynamicExt
	{
	public:
		DCNv2PluginDynamic();

		DCNv2PluginDynamic(const void* data, size_t length, const std::string& name);
		
		DCNv2PluginDynamic(DCNv2Parameters param, const std::string& name);
		
		~DCNv2PluginDynamic() override;

		// IPluginV2DynamicExt Methods
		IPluginV2DynamicExt* clone() const TRT_NOEXCEPT override;
		
		DimsExprs getOutputDimensions(int outputIndex, const DimsExprs* inputs,
			int nbInputs, IExprBuilder& exprBuilder) TRT_NOEXCEPT override;
		
		bool supportsFormatCombination(int pos, const PluginTensorDesc* inOut,
			int nbInputs, int nbOutputs) TRT_NOEXCEPT override;
		
		void configurePlugin(const DynamicPluginTensorDesc* inputs, int nbInputs,
			const DynamicPluginTensorDesc* outputs, int nbOutputs) TRT_NOEXCEPT override;

		size_t getWorkspaceSize(const nvinfer1::PluginTensorDesc* inputs, int nbInputs,
			const nvinfer1::PluginTensorDesc* outputs, int nbOutputs) const TRT_NOEXCEPT override;
		
		int enqueue(const PluginTensorDesc* inputDesc, const PluginTensorDesc* outputDesc,
			const void* const* inputs, void* const* outputs, void* workspace,
			cudaStream_t stream) TRT_NOEXCEPT override;

		// IPluginV2Ext Methods
		DataType getOutputDataType(int index, const DataType* inputTypes,
			int nbInputs) const TRT_NOEXCEPT override;

		void attachToContext(cudnnContext* cudnnContext, cublasContext* cublasContext,
			IGpuAllocator* gpuAllocator) TRT_NOEXCEPT override;

		void detachFromContext() TRT_NOEXCEPT override;

		// IPluginV2 Methods
		const char* getPluginType() const TRT_NOEXCEPT override;

		const char* getPluginVersion() const TRT_NOEXCEPT override;

		int getNbOutputs() const TRT_NOEXCEPT override;

		int initialize() TRT_NOEXCEPT override;

		void terminate() TRT_NOEXCEPT override;

		size_t getSerializationSize() const TRT_NOEXCEPT override;

		void serialize(void* buffer) const TRT_NOEXCEPT override;

		void destroy() TRT_NOEXCEPT override;

		void setPluginNamespace(const char* pluginNamespace) TRT_NOEXCEPT override;

		const char* getPluginNamespace() const TRT_NOEXCEPT override;

	private:
		DCNv2Parameters mParam;
		const std::string mLayerName;
		std::string mNamespace;
		cublasHandle_t cublasHandle_;
		cudnnHandle_t cudnnHandle_;
		DataType mType;
		Dims input1_shape;
		Dims input2_shape;
		Dims weights_shape;
		Dims output_shape;
		Weights mDeviceWeights, mDeviceBiases;

	public:
		using nvinfer1::IPluginV2::getOutputDimensions;
		using nvinfer1::IPluginV2Ext::configurePlugin;
		using nvinfer1::IPluginV2::enqueue;
		using nvinfer1::IPluginV2::getWorkspaceSize;
	}; // class DCNv2Plugin

	class DCNv2PluginDynamicCreator : public IPluginCreator
	{
	public:
		DCNv2PluginDynamicCreator();

		const char* getPluginName() const TRT_NOEXCEPT override;

		const char* getPluginVersion() const TRT_NOEXCEPT override;

		const PluginFieldCollection* getFieldNames() TRT_NOEXCEPT override;

		IPluginV2* createPlugin(const char* name,
			const PluginFieldCollection* fc) TRT_NOEXCEPT override;

		IPluginV2* deserializePlugin(const char* name, const void* serialData, 
			size_t serialLength) TRT_NOEXCEPT override;

		void setPluginNamespace(const char* pluginNamespace) TRT_NOEXCEPT override;

		const char* getPluginNamespace() const TRT_NOEXCEPT override;

	private:
		static PluginFieldCollection mFC;
		static std::vector<PluginField> mPluginAttributes;

		std::string mNamespace;
	}; // class DCNv2PluginDynamicCreator


	REGISTER_TENSORRT_PLUGIN(DCNv2PluginDynamicCreator);
} //namespace nvinfer1

#endif // __TRT_DCNV2_PLUGIN_H__
