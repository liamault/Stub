// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: regulatory_to_broker.proto

#ifndef PROTOBUF_INCLUDED_regulatory_5fto_5fbroker_2eproto
#define PROTOBUF_INCLUDED_regulatory_5fto_5fbroker_2eproto

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3006001
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
#include "common.pb.h"
// @@protoc_insertion_point(includes)
#define PROTOBUF_INTERNAL_EXPORT_protobuf_regulatory_5fto_5fbroker_2eproto 

namespace protobuf_regulatory_5fto_5fbroker_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[2];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
}  // namespace protobuf_regulatory_5fto_5fbroker_2eproto
namespace regulatory_to_broker {
class BrokerageRuleViolationRequest;
class BrokerageRuleViolationRequestDefaultTypeInternal;
extern BrokerageRuleViolationRequestDefaultTypeInternal _BrokerageRuleViolationRequest_default_instance_;
class BrokerageRuleViolationResponse;
class BrokerageRuleViolationResponseDefaultTypeInternal;
extern BrokerageRuleViolationResponseDefaultTypeInternal _BrokerageRuleViolationResponse_default_instance_;
}  // namespace regulatory_to_broker
namespace google {
namespace protobuf {
template<> ::regulatory_to_broker::BrokerageRuleViolationRequest* Arena::CreateMaybeMessage<::regulatory_to_broker::BrokerageRuleViolationRequest>(Arena*);
template<> ::regulatory_to_broker::BrokerageRuleViolationResponse* Arena::CreateMaybeMessage<::regulatory_to_broker::BrokerageRuleViolationResponse>(Arena*);
}  // namespace protobuf
}  // namespace google
namespace regulatory_to_broker {

enum ReasonForBlock {
  Reason_Default = 0,
  ITT_Break_Good_News = 1,
  ITT_Break_Bad_News = 2,
  Trade_Block_Violation = 3,
  Tip_Flag_Active = 4,
  TTV_Exceeds_mTTV = 5,
  ReasonForBlock_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  ReasonForBlock_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
bool ReasonForBlock_IsValid(int value);
const ReasonForBlock ReasonForBlock_MIN = Reason_Default;
const ReasonForBlock ReasonForBlock_MAX = TTV_Exceeds_mTTV;
const int ReasonForBlock_ARRAYSIZE = ReasonForBlock_MAX + 1;

const ::google::protobuf::EnumDescriptor* ReasonForBlock_descriptor();
inline const ::std::string& ReasonForBlock_Name(ReasonForBlock value) {
  return ::google::protobuf::internal::NameOfEnum(
    ReasonForBlock_descriptor(), value);
}
inline bool ReasonForBlock_Parse(
    const ::std::string& name, ReasonForBlock* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ReasonForBlock>(
    ReasonForBlock_descriptor(), name, value);
}
enum ResponseType {
  INVALID = 0,
  OK = 1,
  ERROR = 2,
  ResponseType_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  ResponseType_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
bool ResponseType_IsValid(int value);
const ResponseType ResponseType_MIN = INVALID;
const ResponseType ResponseType_MAX = ERROR;
const int ResponseType_ARRAYSIZE = ResponseType_MAX + 1;

const ::google::protobuf::EnumDescriptor* ResponseType_descriptor();
inline const ::std::string& ResponseType_Name(ResponseType value) {
  return ::google::protobuf::internal::NameOfEnum(
    ResponseType_descriptor(), value);
}
inline bool ResponseType_Parse(
    const ::std::string& name, ResponseType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ResponseType>(
    ResponseType_descriptor(), name, value);
}
// ===================================================================

class BrokerageRuleViolationRequest : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:regulatory_to_broker.BrokerageRuleViolationRequest) */ {
 public:
  BrokerageRuleViolationRequest();
  virtual ~BrokerageRuleViolationRequest();

  BrokerageRuleViolationRequest(const BrokerageRuleViolationRequest& from);

  inline BrokerageRuleViolationRequest& operator=(const BrokerageRuleViolationRequest& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  BrokerageRuleViolationRequest(BrokerageRuleViolationRequest&& from) noexcept
    : BrokerageRuleViolationRequest() {
    *this = ::std::move(from);
  }

  inline BrokerageRuleViolationRequest& operator=(BrokerageRuleViolationRequest&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const BrokerageRuleViolationRequest& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const BrokerageRuleViolationRequest* internal_default_instance() {
    return reinterpret_cast<const BrokerageRuleViolationRequest*>(
               &_BrokerageRuleViolationRequest_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  void Swap(BrokerageRuleViolationRequest* other);
  friend void swap(BrokerageRuleViolationRequest& a, BrokerageRuleViolationRequest& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline BrokerageRuleViolationRequest* New() const final {
    return CreateMaybeMessage<BrokerageRuleViolationRequest>(NULL);
  }

  BrokerageRuleViolationRequest* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<BrokerageRuleViolationRequest>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const BrokerageRuleViolationRequest& from);
  void MergeFrom(const BrokerageRuleViolationRequest& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(BrokerageRuleViolationRequest* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // .Header header = 1;
  bool has_header() const;
  void clear_header();
  static const int kHeaderFieldNumber = 1;
  private:
  const ::Header& _internal_header() const;
  public:
  const ::Header& header() const;
  ::Header* release_header();
  ::Header* mutable_header();
  void set_allocated_header(::Header* header);

  // .BrokerageId brokerage = 2;
  bool has_brokerage() const;
  void clear_brokerage();
  static const int kBrokerageFieldNumber = 2;
  private:
  const ::BrokerageId& _internal_brokerage() const;
  public:
  const ::BrokerageId& brokerage() const;
  ::BrokerageId* release_brokerage();
  ::BrokerageId* mutable_brokerage();
  void set_allocated_brokerage(::BrokerageId* brokerage);

  // uint32 block_duration = 3;
  void clear_block_duration();
  static const int kBlockDurationFieldNumber = 3;
  ::google::protobuf::uint32 block_duration() const;
  void set_block_duration(::google::protobuf::uint32 value);

  // .regulatory_to_broker.ReasonForBlock reason = 4;
  void clear_reason();
  static const int kReasonFieldNumber = 4;
  ::regulatory_to_broker::ReasonForBlock reason() const;
  void set_reason(::regulatory_to_broker::ReasonForBlock value);

  // @@protoc_insertion_point(class_scope:regulatory_to_broker.BrokerageRuleViolationRequest)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::Header* header_;
  ::BrokerageId* brokerage_;
  ::google::protobuf::uint32 block_duration_;
  int reason_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_regulatory_5fto_5fbroker_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class BrokerageRuleViolationResponse : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:regulatory_to_broker.BrokerageRuleViolationResponse) */ {
 public:
  BrokerageRuleViolationResponse();
  virtual ~BrokerageRuleViolationResponse();

  BrokerageRuleViolationResponse(const BrokerageRuleViolationResponse& from);

  inline BrokerageRuleViolationResponse& operator=(const BrokerageRuleViolationResponse& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  BrokerageRuleViolationResponse(BrokerageRuleViolationResponse&& from) noexcept
    : BrokerageRuleViolationResponse() {
    *this = ::std::move(from);
  }

  inline BrokerageRuleViolationResponse& operator=(BrokerageRuleViolationResponse&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const BrokerageRuleViolationResponse& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const BrokerageRuleViolationResponse* internal_default_instance() {
    return reinterpret_cast<const BrokerageRuleViolationResponse*>(
               &_BrokerageRuleViolationResponse_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  void Swap(BrokerageRuleViolationResponse* other);
  friend void swap(BrokerageRuleViolationResponse& a, BrokerageRuleViolationResponse& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline BrokerageRuleViolationResponse* New() const final {
    return CreateMaybeMessage<BrokerageRuleViolationResponse>(NULL);
  }

  BrokerageRuleViolationResponse* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<BrokerageRuleViolationResponse>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const BrokerageRuleViolationResponse& from);
  void MergeFrom(const BrokerageRuleViolationResponse& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(BrokerageRuleViolationResponse* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // .Header header = 1;
  bool has_header() const;
  void clear_header();
  static const int kHeaderFieldNumber = 1;
  private:
  const ::Header& _internal_header() const;
  public:
  const ::Header& header() const;
  ::Header* release_header();
  ::Header* mutable_header();
  void set_allocated_header(::Header* header);

  // .regulatory_to_broker.ResponseType response = 2;
  void clear_response();
  static const int kResponseFieldNumber = 2;
  ::regulatory_to_broker::ResponseType response() const;
  void set_response(::regulatory_to_broker::ResponseType value);

  // @@protoc_insertion_point(class_scope:regulatory_to_broker.BrokerageRuleViolationResponse)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::Header* header_;
  int response_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_regulatory_5fto_5fbroker_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// BrokerageRuleViolationRequest

// .Header header = 1;
inline bool BrokerageRuleViolationRequest::has_header() const {
  return this != internal_default_instance() && header_ != NULL;
}
inline const ::Header& BrokerageRuleViolationRequest::_internal_header() const {
  return *header_;
}
inline const ::Header& BrokerageRuleViolationRequest::header() const {
  const ::Header* p = header_;
  // @@protoc_insertion_point(field_get:regulatory_to_broker.BrokerageRuleViolationRequest.header)
  return p != NULL ? *p : *reinterpret_cast<const ::Header*>(
      &::_Header_default_instance_);
}
inline ::Header* BrokerageRuleViolationRequest::release_header() {
  // @@protoc_insertion_point(field_release:regulatory_to_broker.BrokerageRuleViolationRequest.header)
  
  ::Header* temp = header_;
  header_ = NULL;
  return temp;
}
inline ::Header* BrokerageRuleViolationRequest::mutable_header() {
  
  if (header_ == NULL) {
    auto* p = CreateMaybeMessage<::Header>(GetArenaNoVirtual());
    header_ = p;
  }
  // @@protoc_insertion_point(field_mutable:regulatory_to_broker.BrokerageRuleViolationRequest.header)
  return header_;
}
inline void BrokerageRuleViolationRequest::set_allocated_header(::Header* header) {
  ::google::protobuf::Arena* message_arena = GetArenaNoVirtual();
  if (message_arena == NULL) {
    delete reinterpret_cast< ::google::protobuf::MessageLite*>(header_);
  }
  if (header) {
    ::google::protobuf::Arena* submessage_arena = NULL;
    if (message_arena != submessage_arena) {
      header = ::google::protobuf::internal::GetOwnedMessage(
          message_arena, header, submessage_arena);
    }
    
  } else {
    
  }
  header_ = header;
  // @@protoc_insertion_point(field_set_allocated:regulatory_to_broker.BrokerageRuleViolationRequest.header)
}

// .BrokerageId brokerage = 2;
inline bool BrokerageRuleViolationRequest::has_brokerage() const {
  return this != internal_default_instance() && brokerage_ != NULL;
}
inline const ::BrokerageId& BrokerageRuleViolationRequest::_internal_brokerage() const {
  return *brokerage_;
}
inline const ::BrokerageId& BrokerageRuleViolationRequest::brokerage() const {
  const ::BrokerageId* p = brokerage_;
  // @@protoc_insertion_point(field_get:regulatory_to_broker.BrokerageRuleViolationRequest.brokerage)
  return p != NULL ? *p : *reinterpret_cast<const ::BrokerageId*>(
      &::_BrokerageId_default_instance_);
}
inline ::BrokerageId* BrokerageRuleViolationRequest::release_brokerage() {
  // @@protoc_insertion_point(field_release:regulatory_to_broker.BrokerageRuleViolationRequest.brokerage)
  
  ::BrokerageId* temp = brokerage_;
  brokerage_ = NULL;
  return temp;
}
inline ::BrokerageId* BrokerageRuleViolationRequest::mutable_brokerage() {
  
  if (brokerage_ == NULL) {
    auto* p = CreateMaybeMessage<::BrokerageId>(GetArenaNoVirtual());
    brokerage_ = p;
  }
  // @@protoc_insertion_point(field_mutable:regulatory_to_broker.BrokerageRuleViolationRequest.brokerage)
  return brokerage_;
}
inline void BrokerageRuleViolationRequest::set_allocated_brokerage(::BrokerageId* brokerage) {
  ::google::protobuf::Arena* message_arena = GetArenaNoVirtual();
  if (message_arena == NULL) {
    delete reinterpret_cast< ::google::protobuf::MessageLite*>(brokerage_);
  }
  if (brokerage) {
    ::google::protobuf::Arena* submessage_arena = NULL;
    if (message_arena != submessage_arena) {
      brokerage = ::google::protobuf::internal::GetOwnedMessage(
          message_arena, brokerage, submessage_arena);
    }
    
  } else {
    
  }
  brokerage_ = brokerage;
  // @@protoc_insertion_point(field_set_allocated:regulatory_to_broker.BrokerageRuleViolationRequest.brokerage)
}

// uint32 block_duration = 3;
inline void BrokerageRuleViolationRequest::clear_block_duration() {
  block_duration_ = 0u;
}
inline ::google::protobuf::uint32 BrokerageRuleViolationRequest::block_duration() const {
  // @@protoc_insertion_point(field_get:regulatory_to_broker.BrokerageRuleViolationRequest.block_duration)
  return block_duration_;
}
inline void BrokerageRuleViolationRequest::set_block_duration(::google::protobuf::uint32 value) {
  
  block_duration_ = value;
  // @@protoc_insertion_point(field_set:regulatory_to_broker.BrokerageRuleViolationRequest.block_duration)
}

// .regulatory_to_broker.ReasonForBlock reason = 4;
inline void BrokerageRuleViolationRequest::clear_reason() {
  reason_ = 0;
}
inline ::regulatory_to_broker::ReasonForBlock BrokerageRuleViolationRequest::reason() const {
  // @@protoc_insertion_point(field_get:regulatory_to_broker.BrokerageRuleViolationRequest.reason)
  return static_cast< ::regulatory_to_broker::ReasonForBlock >(reason_);
}
inline void BrokerageRuleViolationRequest::set_reason(::regulatory_to_broker::ReasonForBlock value) {
  
  reason_ = value;
  // @@protoc_insertion_point(field_set:regulatory_to_broker.BrokerageRuleViolationRequest.reason)
}

// -------------------------------------------------------------------

// BrokerageRuleViolationResponse

// .Header header = 1;
inline bool BrokerageRuleViolationResponse::has_header() const {
  return this != internal_default_instance() && header_ != NULL;
}
inline const ::Header& BrokerageRuleViolationResponse::_internal_header() const {
  return *header_;
}
inline const ::Header& BrokerageRuleViolationResponse::header() const {
  const ::Header* p = header_;
  // @@protoc_insertion_point(field_get:regulatory_to_broker.BrokerageRuleViolationResponse.header)
  return p != NULL ? *p : *reinterpret_cast<const ::Header*>(
      &::_Header_default_instance_);
}
inline ::Header* BrokerageRuleViolationResponse::release_header() {
  // @@protoc_insertion_point(field_release:regulatory_to_broker.BrokerageRuleViolationResponse.header)
  
  ::Header* temp = header_;
  header_ = NULL;
  return temp;
}
inline ::Header* BrokerageRuleViolationResponse::mutable_header() {
  
  if (header_ == NULL) {
    auto* p = CreateMaybeMessage<::Header>(GetArenaNoVirtual());
    header_ = p;
  }
  // @@protoc_insertion_point(field_mutable:regulatory_to_broker.BrokerageRuleViolationResponse.header)
  return header_;
}
inline void BrokerageRuleViolationResponse::set_allocated_header(::Header* header) {
  ::google::protobuf::Arena* message_arena = GetArenaNoVirtual();
  if (message_arena == NULL) {
    delete reinterpret_cast< ::google::protobuf::MessageLite*>(header_);
  }
  if (header) {
    ::google::protobuf::Arena* submessage_arena = NULL;
    if (message_arena != submessage_arena) {
      header = ::google::protobuf::internal::GetOwnedMessage(
          message_arena, header, submessage_arena);
    }
    
  } else {
    
  }
  header_ = header;
  // @@protoc_insertion_point(field_set_allocated:regulatory_to_broker.BrokerageRuleViolationResponse.header)
}

// .regulatory_to_broker.ResponseType response = 2;
inline void BrokerageRuleViolationResponse::clear_response() {
  response_ = 0;
}
inline ::regulatory_to_broker::ResponseType BrokerageRuleViolationResponse::response() const {
  // @@protoc_insertion_point(field_get:regulatory_to_broker.BrokerageRuleViolationResponse.response)
  return static_cast< ::regulatory_to_broker::ResponseType >(response_);
}
inline void BrokerageRuleViolationResponse::set_response(::regulatory_to_broker::ResponseType value) {
  
  response_ = value;
  // @@protoc_insertion_point(field_set:regulatory_to_broker.BrokerageRuleViolationResponse.response)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace regulatory_to_broker

namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::regulatory_to_broker::ReasonForBlock> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::regulatory_to_broker::ReasonForBlock>() {
  return ::regulatory_to_broker::ReasonForBlock_descriptor();
}
template <> struct is_proto_enum< ::regulatory_to_broker::ResponseType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::regulatory_to_broker::ResponseType>() {
  return ::regulatory_to_broker::ResponseType_descriptor();
}

}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_INCLUDED_regulatory_5fto_5fbroker_2eproto
