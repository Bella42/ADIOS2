/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * BP3Deserializer.h
 *
 *  Created on: Sep 7, 2017
 *      Author: William F Godoy godoywf@ornl.gov
 */

#ifndef ADIOS2_TOOLKIT_FORMAT_BP3_BP3DESERIALIZER_H_
#define ADIOS2_TOOLKIT_FORMAT_BP3_BP3DESERIALIZER_H_

#include <mutex>
#include <set>

#include "adios2/core/IO.h"
#include "adios2/core/Variable.h"
#include "adios2/helper/adiosFunctions.h" //VariablesSubFileInfo
#include "adios2/toolkit/format/bp3/BP3Base.h"

namespace adios2
{
namespace format
{

class BP3Deserializer : public BP3Base
{

public:
    /** BP Minifooter fields */
    Minifooter m_Minifooter;

    /**
     * Unique constructor
     * @param mpiComm
     * @param debug true: extra checks
     */
    BP3Deserializer(MPI_Comm mpiComm, const bool debugMode);

    ~BP3Deserializer() = default;

    void ParseMetadata(const BufferSTL &bufferSTL, core::IO &io);

    /**
     * Used to get the variable payload data for the current selection (dims and
     * steps), used in single buffer for streaming
     * @param variable
     * @param bufferSTL bp buffer input that contains metadata and data
     */
    template <class T>
    void GetSyncVariableDataFromStream(core::Variable<T> &variable,
                                       BufferSTL &bufferSTL) const;

    /**
     * Initializes a block inside variable.m_BlocksInfo
     * @param variable input
     * @param data user data pointer
     * @return a reference inside variable.m_BlocksInfo (invalidated if called
     * twice)
     */
    template <class T>
    typename core::Variable<T>::Info &
    InitVariableBlockInfo(core::Variable<T> &variable, T *data);

    /**
     * Sets read block information from the available metadata information
     * @param variable
     * @param blockInfo
     */
    template <class T>
    void SetVariableBlockInfo(core::Variable<T> &variable,
                              typename core::Variable<T>::Info &blockInfo);

    /**
     * Clips and assigns memory to blockInfo.Data from a contiguous memory input
     * @param blockInfo
     * @param contiguousMemory
     * @param blockBox
     * @param intersectionBox
     */
    template <class T>
    void ClipContiguousMemory(typename core::Variable<T>::Info &blockInfo,
                              const std::vector<char> &contiguousMemory,
                              const Box<Dims> &blockBox,
                              const Box<Dims> &intersectionBox) const;

    /**
     * Gets a value directly from metadata (if Variable is single value)
     * @param variable
     * @param data
     */
    template <class T>
    void GetValueFromMetadata(core::Variable<T> &variable, T *data) const;

    // TODO : Will deprecate all function below
    std::map<std::string, helper::SubFileInfoMap>
    PerformGetsVariablesSubFileInfo(core::IO &io);

    // TODO : will deprecate
    template <class T>
    std::map<std::string, helper::SubFileInfoMap>
    GetSyncVariableSubFileInfo(const core::Variable<T> &variable) const;

    // TODO : will deprecate
    template <class T>
    void GetDeferredVariable(core::Variable<T> &variable, T *data);

    // TODO : will deprecate
    template <class T>
    helper::SubFileInfoMap
    GetSubFileInfo(const core::Variable<T> &variable) const;

    // TODO : will deprecate
    void ClipMemory(const std::string &variableName, core::IO &io,
                    const std::vector<char> &contiguousMemory,
                    const Box<Dims> &blockBox,
                    const Box<Dims> &intersectionBox) const;

    // TODO: will deprecate
    bool m_PerformedGets = false;

private:
    std::map<std::string, helper::SubFileInfoMap> m_DeferredVariablesMap;

    static std::mutex m_Mutex;

    void ParseMinifooter(const BufferSTL &bufferSTL);
    void ParsePGIndex(const BufferSTL &bufferSTL, const core::IO &io);
    void ParseVariablesIndex(const BufferSTL &bufferSTL, core::IO &io);
    void ParseAttributesIndex(const BufferSTL &bufferSTL, core::IO &io);

    /**
     * Reads a variable index element (serialized) and calls IO.DefineVariable
     * to deserialize the Variable metadata
     * @param header serialize
     * @param io
     * @param buffer
     * @param position
     */
    template <class T>
    void DefineVariableInIO(const ElementIndexHeader &header, core::IO &io,
                            const std::vector<char> &buffer,
                            size_t position) const;

    template <class T>
    void DefineAttributeInIO(const ElementIndexHeader &header, core::IO &io,
                             const std::vector<char> &buffer,
                             size_t position) const;

    template <class T>
    void GetValueFromMetadataCommon(core::Variable<T> &variable, T *data) const;
};

#define declare_template_instantiation(T)                                      \
    extern template void BP3Deserializer::GetSyncVariableDataFromStream(       \
        core::Variable<T> &, BufferSTL &) const;                               \
                                                                               \
    extern template typename core::Variable<T>::Info &                         \
    BP3Deserializer::InitVariableBlockInfo(core::Variable<T> &, T *);          \
                                                                               \
    extern template void BP3Deserializer::SetVariableBlockInfo(                \
        core::Variable<T> &, typename core::Variable<T>::Info &);              \
                                                                               \
    extern template void BP3Deserializer::ClipContiguousMemory<T>(             \
        typename core::Variable<T>::Info &, const std::vector<char> &,         \
        const Box<Dims> &, const Box<Dims> &intersectionBox) const;            \
                                                                               \
    extern template void BP3Deserializer::GetValueFromMetadata(                \
        core::Variable<T> &variable, T *) const;

ADIOS2_FOREACH_TYPE_1ARG(declare_template_instantiation)
#undef declare_template_instantiation

#define declare_template_instantiation(T)                                      \
                                                                               \
    extern template std::map<std::string, helper::SubFileInfoMap>              \
    BP3Deserializer::GetSyncVariableSubFileInfo(const core::Variable<T> &)     \
        const;                                                                 \
                                                                               \
    extern template void BP3Deserializer::GetDeferredVariable(                 \
        core::Variable<T> &, T *);                                             \
                                                                               \
    extern template helper::SubFileInfoMap BP3Deserializer::GetSubFileInfo(    \
        const core::Variable<T> &) const;

ADIOS2_FOREACH_TYPE_1ARG(declare_template_instantiation)
#undef declare_template_instantiation

} // end namespace format
} // end namespace adios2

#endif /* ADIOS2_TOOLKIT_FORMAT_BP3_BP3DESERIALIZER_H_ */
