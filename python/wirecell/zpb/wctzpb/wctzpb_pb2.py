# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: src/wctzpb.proto

import sys
_b=sys.version_info[0]<3 and (lambda x:x) or (lambda x:x.encode('latin1'))
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


from google.protobuf import any_pb2 as google_dot_protobuf_dot_any__pb2


DESCRIPTOR = _descriptor.FileDescriptor(
  name='src/wctzpb.proto',
  package='wctzpb',
  syntax='proto3',
  serialized_options=None,
  serialized_pb=_b('\n\x10src/wctzpb.proto\x12\x06wctzpb\x1a\x19google/protobuf/any.proto\"0\n\x07Payload\x12%\n\x07objects\x18\x01 \x03(\x0b\x32\x14.google.protobuf.Any\"(\n\x05Point\x12\t\n\x01x\x18\x01 \x01(\x02\x12\t\n\x01y\x18\x02 \x01(\x02\x12\t\n\x01z\x18\x03 \x01(\x02\"\xb6\x01\n\x04\x44\x65po\x12\r\n\x05ident\x18\x01 \x01(\x04\x12\x1a\n\x03pos\x18\x02 \x01(\x0b\x32\r.wctzpb.Point\x12\x0c\n\x04time\x18\x03 \x01(\x01\x12\x0e\n\x06\x63harge\x18\x04 \x01(\x01\x12\x0e\n\x06\x65nergy\x18\x05 \x01(\x01\x12\x0f\n\x07trackid\x18\x06 \x01(\r\x12\x0b\n\x03pdg\x18\x07 \x01(\r\x12\r\n\x05prior\x18\x08 \x01(\x04\x12\x13\n\x0b\x65xtent_long\x18\t \x01(\x01\x12\x13\n\x0b\x65xtent_tran\x18\n \x01(\x01\"5\n\x07\x44\x65poSet\x12\r\n\x05ident\x18\x01 \x01(\r\x12\x1b\n\x05\x64\x65pos\x18\x02 \x03(\x0b\x32\x0c.wctzpb.Depo\"\x1e\n\nFloatArray\x12\x10\n\x08\x65lements\x18\x01 \x03(\x02\"\x1c\n\x08IntArray\x12\x10\n\x08\x65lements\x18\x01 \x03(\x05\"K\n\x05Trace\x12\x0f\n\x07\x63hannel\x18\x01 \x01(\x05\x12\x0c\n\x04tbin\x18\x02 \x01(\x05\x12#\n\x07samples\x18\x03 \x01(\x0b\x32\x12.wctzpb.FloatArray\"(\n\x0c\x42inRangeList\x12\x0b\n\x03\x62\x65g\x18\x01 \x03(\x05\x12\x0b\n\x03\x65nd\x18\x02 \x03(\x05\"\x9c\x01\n\x0c\x43hannelMasks\x12@\n\x0f\x62in_range_lists\x18\x01 \x03(\x0b\x32\'.wctzpb.ChannelMasks.BinRangeListsEntry\x1aJ\n\x12\x42inRangeListsEntry\x12\x0b\n\x03key\x18\x01 \x01(\x05\x12#\n\x05value\x18\x02 \x01(\x0b\x32\x14.wctzpb.BinRangeList:\x02\x38\x01\"\xee\x03\n\x05\x46rame\x12\r\n\x05ident\x18\x01 \x01(\x05\x12\x0c\n\x04time\x18\x02 \x01(\x01\x12\x0c\n\x04tick\x18\x03 \x01(\x01\x12\x1d\n\x06traces\x18\x04 \x03(\x0b\x32\r.wctzpb.Trace\x12\x12\n\nframe_tags\x18\x05 \x03(\t\x12\x36\n\rtagged_traces\x18\x06 \x03(\x0b\x32\x1f.wctzpb.Frame.TaggedTracesEntry\x12:\n\x0ftrace_summaries\x18\x07 \x03(\x0b\x32!.wctzpb.Frame.TraceSummariesEntry\x12\x36\n\rchannel_masks\x18\x08 \x03(\x0b\x32\x1f.wctzpb.Frame.ChannelMasksEntry\x1a\x45\n\x11TaggedTracesEntry\x12\x0b\n\x03key\x18\x01 \x01(\t\x12\x1f\n\x05value\x18\x02 \x01(\x0b\x32\x10.wctzpb.IntArray:\x02\x38\x01\x1aI\n\x13TraceSummariesEntry\x12\x0b\n\x03key\x18\x01 \x01(\t\x12!\n\x05value\x18\x02 \x01(\x0b\x32\x12.wctzpb.FloatArray:\x02\x38\x01\x1aI\n\x11\x43hannelMasksEntry\x12\x0b\n\x03key\x18\x01 \x01(\t\x12#\n\x05value\x18\x02 \x01(\x0b\x32\x14.wctzpb.ChannelMasks:\x02\x38\x01\x62\x06proto3')
  ,
  dependencies=[google_dot_protobuf_dot_any__pb2.DESCRIPTOR,])




_PAYLOAD = _descriptor.Descriptor(
  name='Payload',
  full_name='wctzpb.Payload',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='objects', full_name='wctzpb.Payload.objects', index=0,
      number=1, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=55,
  serialized_end=103,
)


_POINT = _descriptor.Descriptor(
  name='Point',
  full_name='wctzpb.Point',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='x', full_name='wctzpb.Point.x', index=0,
      number=1, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='y', full_name='wctzpb.Point.y', index=1,
      number=2, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='z', full_name='wctzpb.Point.z', index=2,
      number=3, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=105,
  serialized_end=145,
)


_DEPO = _descriptor.Descriptor(
  name='Depo',
  full_name='wctzpb.Depo',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='ident', full_name='wctzpb.Depo.ident', index=0,
      number=1, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='pos', full_name='wctzpb.Depo.pos', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='time', full_name='wctzpb.Depo.time', index=2,
      number=3, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='charge', full_name='wctzpb.Depo.charge', index=3,
      number=4, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='energy', full_name='wctzpb.Depo.energy', index=4,
      number=5, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='trackid', full_name='wctzpb.Depo.trackid', index=5,
      number=6, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='pdg', full_name='wctzpb.Depo.pdg', index=6,
      number=7, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='prior', full_name='wctzpb.Depo.prior', index=7,
      number=8, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='extent_long', full_name='wctzpb.Depo.extent_long', index=8,
      number=9, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='extent_tran', full_name='wctzpb.Depo.extent_tran', index=9,
      number=10, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=148,
  serialized_end=330,
)


_DEPOSET = _descriptor.Descriptor(
  name='DepoSet',
  full_name='wctzpb.DepoSet',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='ident', full_name='wctzpb.DepoSet.ident', index=0,
      number=1, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='depos', full_name='wctzpb.DepoSet.depos', index=1,
      number=2, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=332,
  serialized_end=385,
)


_FLOATARRAY = _descriptor.Descriptor(
  name='FloatArray',
  full_name='wctzpb.FloatArray',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='elements', full_name='wctzpb.FloatArray.elements', index=0,
      number=1, type=2, cpp_type=6, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=387,
  serialized_end=417,
)


_INTARRAY = _descriptor.Descriptor(
  name='IntArray',
  full_name='wctzpb.IntArray',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='elements', full_name='wctzpb.IntArray.elements', index=0,
      number=1, type=5, cpp_type=1, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=419,
  serialized_end=447,
)


_TRACE = _descriptor.Descriptor(
  name='Trace',
  full_name='wctzpb.Trace',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='channel', full_name='wctzpb.Trace.channel', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='tbin', full_name='wctzpb.Trace.tbin', index=1,
      number=2, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='samples', full_name='wctzpb.Trace.samples', index=2,
      number=3, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=449,
  serialized_end=524,
)


_BINRANGELIST = _descriptor.Descriptor(
  name='BinRangeList',
  full_name='wctzpb.BinRangeList',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='beg', full_name='wctzpb.BinRangeList.beg', index=0,
      number=1, type=5, cpp_type=1, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='end', full_name='wctzpb.BinRangeList.end', index=1,
      number=2, type=5, cpp_type=1, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=526,
  serialized_end=566,
)


_CHANNELMASKS_BINRANGELISTSENTRY = _descriptor.Descriptor(
  name='BinRangeListsEntry',
  full_name='wctzpb.ChannelMasks.BinRangeListsEntry',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='key', full_name='wctzpb.ChannelMasks.BinRangeListsEntry.key', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='value', full_name='wctzpb.ChannelMasks.BinRangeListsEntry.value', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=_b('8\001'),
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=651,
  serialized_end=725,
)

_CHANNELMASKS = _descriptor.Descriptor(
  name='ChannelMasks',
  full_name='wctzpb.ChannelMasks',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='bin_range_lists', full_name='wctzpb.ChannelMasks.bin_range_lists', index=0,
      number=1, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[_CHANNELMASKS_BINRANGELISTSENTRY, ],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=569,
  serialized_end=725,
)


_FRAME_TAGGEDTRACESENTRY = _descriptor.Descriptor(
  name='TaggedTracesEntry',
  full_name='wctzpb.Frame.TaggedTracesEntry',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='key', full_name='wctzpb.Frame.TaggedTracesEntry.key', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='value', full_name='wctzpb.Frame.TaggedTracesEntry.value', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=_b('8\001'),
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=1003,
  serialized_end=1072,
)

_FRAME_TRACESUMMARIESENTRY = _descriptor.Descriptor(
  name='TraceSummariesEntry',
  full_name='wctzpb.Frame.TraceSummariesEntry',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='key', full_name='wctzpb.Frame.TraceSummariesEntry.key', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='value', full_name='wctzpb.Frame.TraceSummariesEntry.value', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=_b('8\001'),
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=1074,
  serialized_end=1147,
)

_FRAME_CHANNELMASKSENTRY = _descriptor.Descriptor(
  name='ChannelMasksEntry',
  full_name='wctzpb.Frame.ChannelMasksEntry',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='key', full_name='wctzpb.Frame.ChannelMasksEntry.key', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='value', full_name='wctzpb.Frame.ChannelMasksEntry.value', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=_b('8\001'),
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=1149,
  serialized_end=1222,
)

_FRAME = _descriptor.Descriptor(
  name='Frame',
  full_name='wctzpb.Frame',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='ident', full_name='wctzpb.Frame.ident', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='time', full_name='wctzpb.Frame.time', index=1,
      number=2, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='tick', full_name='wctzpb.Frame.tick', index=2,
      number=3, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='traces', full_name='wctzpb.Frame.traces', index=3,
      number=4, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='frame_tags', full_name='wctzpb.Frame.frame_tags', index=4,
      number=5, type=9, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='tagged_traces', full_name='wctzpb.Frame.tagged_traces', index=5,
      number=6, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='trace_summaries', full_name='wctzpb.Frame.trace_summaries', index=6,
      number=7, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='channel_masks', full_name='wctzpb.Frame.channel_masks', index=7,
      number=8, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[_FRAME_TAGGEDTRACESENTRY, _FRAME_TRACESUMMARIESENTRY, _FRAME_CHANNELMASKSENTRY, ],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=728,
  serialized_end=1222,
)

_PAYLOAD.fields_by_name['objects'].message_type = google_dot_protobuf_dot_any__pb2._ANY
_DEPO.fields_by_name['pos'].message_type = _POINT
_DEPOSET.fields_by_name['depos'].message_type = _DEPO
_TRACE.fields_by_name['samples'].message_type = _FLOATARRAY
_CHANNELMASKS_BINRANGELISTSENTRY.fields_by_name['value'].message_type = _BINRANGELIST
_CHANNELMASKS_BINRANGELISTSENTRY.containing_type = _CHANNELMASKS
_CHANNELMASKS.fields_by_name['bin_range_lists'].message_type = _CHANNELMASKS_BINRANGELISTSENTRY
_FRAME_TAGGEDTRACESENTRY.fields_by_name['value'].message_type = _INTARRAY
_FRAME_TAGGEDTRACESENTRY.containing_type = _FRAME
_FRAME_TRACESUMMARIESENTRY.fields_by_name['value'].message_type = _FLOATARRAY
_FRAME_TRACESUMMARIESENTRY.containing_type = _FRAME
_FRAME_CHANNELMASKSENTRY.fields_by_name['value'].message_type = _CHANNELMASKS
_FRAME_CHANNELMASKSENTRY.containing_type = _FRAME
_FRAME.fields_by_name['traces'].message_type = _TRACE
_FRAME.fields_by_name['tagged_traces'].message_type = _FRAME_TAGGEDTRACESENTRY
_FRAME.fields_by_name['trace_summaries'].message_type = _FRAME_TRACESUMMARIESENTRY
_FRAME.fields_by_name['channel_masks'].message_type = _FRAME_CHANNELMASKSENTRY
DESCRIPTOR.message_types_by_name['Payload'] = _PAYLOAD
DESCRIPTOR.message_types_by_name['Point'] = _POINT
DESCRIPTOR.message_types_by_name['Depo'] = _DEPO
DESCRIPTOR.message_types_by_name['DepoSet'] = _DEPOSET
DESCRIPTOR.message_types_by_name['FloatArray'] = _FLOATARRAY
DESCRIPTOR.message_types_by_name['IntArray'] = _INTARRAY
DESCRIPTOR.message_types_by_name['Trace'] = _TRACE
DESCRIPTOR.message_types_by_name['BinRangeList'] = _BINRANGELIST
DESCRIPTOR.message_types_by_name['ChannelMasks'] = _CHANNELMASKS
DESCRIPTOR.message_types_by_name['Frame'] = _FRAME
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

Payload = _reflection.GeneratedProtocolMessageType('Payload', (_message.Message,), dict(
  DESCRIPTOR = _PAYLOAD,
  __module__ = 'src.wctzpb_pb2'
  # @@protoc_insertion_point(class_scope:wctzpb.Payload)
  ))
_sym_db.RegisterMessage(Payload)

Point = _reflection.GeneratedProtocolMessageType('Point', (_message.Message,), dict(
  DESCRIPTOR = _POINT,
  __module__ = 'src.wctzpb_pb2'
  # @@protoc_insertion_point(class_scope:wctzpb.Point)
  ))
_sym_db.RegisterMessage(Point)

Depo = _reflection.GeneratedProtocolMessageType('Depo', (_message.Message,), dict(
  DESCRIPTOR = _DEPO,
  __module__ = 'src.wctzpb_pb2'
  # @@protoc_insertion_point(class_scope:wctzpb.Depo)
  ))
_sym_db.RegisterMessage(Depo)

DepoSet = _reflection.GeneratedProtocolMessageType('DepoSet', (_message.Message,), dict(
  DESCRIPTOR = _DEPOSET,
  __module__ = 'src.wctzpb_pb2'
  # @@protoc_insertion_point(class_scope:wctzpb.DepoSet)
  ))
_sym_db.RegisterMessage(DepoSet)

FloatArray = _reflection.GeneratedProtocolMessageType('FloatArray', (_message.Message,), dict(
  DESCRIPTOR = _FLOATARRAY,
  __module__ = 'src.wctzpb_pb2'
  # @@protoc_insertion_point(class_scope:wctzpb.FloatArray)
  ))
_sym_db.RegisterMessage(FloatArray)

IntArray = _reflection.GeneratedProtocolMessageType('IntArray', (_message.Message,), dict(
  DESCRIPTOR = _INTARRAY,
  __module__ = 'src.wctzpb_pb2'
  # @@protoc_insertion_point(class_scope:wctzpb.IntArray)
  ))
_sym_db.RegisterMessage(IntArray)

Trace = _reflection.GeneratedProtocolMessageType('Trace', (_message.Message,), dict(
  DESCRIPTOR = _TRACE,
  __module__ = 'src.wctzpb_pb2'
  # @@protoc_insertion_point(class_scope:wctzpb.Trace)
  ))
_sym_db.RegisterMessage(Trace)

BinRangeList = _reflection.GeneratedProtocolMessageType('BinRangeList', (_message.Message,), dict(
  DESCRIPTOR = _BINRANGELIST,
  __module__ = 'src.wctzpb_pb2'
  # @@protoc_insertion_point(class_scope:wctzpb.BinRangeList)
  ))
_sym_db.RegisterMessage(BinRangeList)

ChannelMasks = _reflection.GeneratedProtocolMessageType('ChannelMasks', (_message.Message,), dict(

  BinRangeListsEntry = _reflection.GeneratedProtocolMessageType('BinRangeListsEntry', (_message.Message,), dict(
    DESCRIPTOR = _CHANNELMASKS_BINRANGELISTSENTRY,
    __module__ = 'src.wctzpb_pb2'
    # @@protoc_insertion_point(class_scope:wctzpb.ChannelMasks.BinRangeListsEntry)
    ))
  ,
  DESCRIPTOR = _CHANNELMASKS,
  __module__ = 'src.wctzpb_pb2'
  # @@protoc_insertion_point(class_scope:wctzpb.ChannelMasks)
  ))
_sym_db.RegisterMessage(ChannelMasks)
_sym_db.RegisterMessage(ChannelMasks.BinRangeListsEntry)

Frame = _reflection.GeneratedProtocolMessageType('Frame', (_message.Message,), dict(

  TaggedTracesEntry = _reflection.GeneratedProtocolMessageType('TaggedTracesEntry', (_message.Message,), dict(
    DESCRIPTOR = _FRAME_TAGGEDTRACESENTRY,
    __module__ = 'src.wctzpb_pb2'
    # @@protoc_insertion_point(class_scope:wctzpb.Frame.TaggedTracesEntry)
    ))
  ,

  TraceSummariesEntry = _reflection.GeneratedProtocolMessageType('TraceSummariesEntry', (_message.Message,), dict(
    DESCRIPTOR = _FRAME_TRACESUMMARIESENTRY,
    __module__ = 'src.wctzpb_pb2'
    # @@protoc_insertion_point(class_scope:wctzpb.Frame.TraceSummariesEntry)
    ))
  ,

  ChannelMasksEntry = _reflection.GeneratedProtocolMessageType('ChannelMasksEntry', (_message.Message,), dict(
    DESCRIPTOR = _FRAME_CHANNELMASKSENTRY,
    __module__ = 'src.wctzpb_pb2'
    # @@protoc_insertion_point(class_scope:wctzpb.Frame.ChannelMasksEntry)
    ))
  ,
  DESCRIPTOR = _FRAME,
  __module__ = 'src.wctzpb_pb2'
  # @@protoc_insertion_point(class_scope:wctzpb.Frame)
  ))
_sym_db.RegisterMessage(Frame)
_sym_db.RegisterMessage(Frame.TaggedTracesEntry)
_sym_db.RegisterMessage(Frame.TraceSummariesEntry)
_sym_db.RegisterMessage(Frame.ChannelMasksEntry)


_CHANNELMASKS_BINRANGELISTSENTRY._options = None
_FRAME_TAGGEDTRACESENTRY._options = None
_FRAME_TRACESUMMARIESENTRY._options = None
_FRAME_CHANNELMASKSENTRY._options = None
# @@protoc_insertion_point(module_scope)
