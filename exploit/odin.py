#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2017 Oran Avraham <contact@oranav.me>
#
# Distributed under terms of the GPLv3 license.
import usb1
import struct
import time
import logging


class Odin(object):
    TIMEOUT = 1000

    def __init__(self):
        self.context = None
        self.device = None
        self.handle = None
        self.interface = 1
        self.inEndpoint = 0x81
        self.outEndpoint = 0x02

    def __del__(self):
        self.close()

    def open(self):
        self.context = usb1.USBContext().open()
        for i in range(10):
            try:
                self.handle = self.context.openByVendorIDAndProductID(
                    0x04e8, 0x685d)
                break
            except usb1.USBError:
                logging.warning('Open USB device failed, retrying...')
                time.sleep(1)
        self.handle.claimInterface(self.interface)
        self.write(b'ODIN')
        try:
            response = self.read(7)
            if response != b'LOKE':
                raise Exception('Strange response: {}'.format(response))
        except usb1.USBErrorTimeout:
            logging.warning('Protocol initialization failed. Maybe device is '
                            'already initialized?')

    def close(self):
        if self.handle is not None:
            self.handle.close()
            self.handle = None
        if self.context is not None:
            self.context.close()
            self.context = None

    def write(self, buf):
        if isinstance(buf, Packet):
            buf = bytes(buf)
        self.handle.bulkWrite(self.outEndpoint, buf, self.TIMEOUT)

    def read(self, size):
        ret = self.handle.bulkRead(self.inEndpoint, size, self.TIMEOUT)
        ret = bytes(ret)
        return ret


class Packet(object):
    @classmethod
    def pack32(cls, val):
        return struct.pack("<I", val)

    @classmethod
    def unpack32(cls, val):
        return struct.unpack("<I", val)[0]

    def __bytes__(self):
        return self.get_data().ljust(1024, b'\0')

    @classmethod
    def from_buf(cls, buf):
        return Packet()


class ControlPacket(Packet):
    TYPE_SESSION = 0x64
    TYPE_PIT_FILE = 0x65
    TYPE_FILE_TRANSFER = 0x66
    TYPE_END_SESSION = 0x67

    _TYPES_ = {}

    @classmethod
    def register_type(cls, pkt_type, type):
        cls._TYPES_[pkt_type] = type

    def __init__(self, pkt_type):
        self.pkt_type = pkt_type
        self.data_size = 4

    def get_data(self):
        return self.pack32(self.pkt_type)

    @classmethod
    def from_buf(cls, buf):
        pkt_type = cls.unpack32(buf[:4])
        return cls._TYPES_[pkt_type].from_buf(buf[4:])


class SessionSetupPacket(ControlPacket):
    REQUEST_BEGIN_SESSION = 0
    REQUEST_DEVICE_TYPE = 1
    REQUEST_TOTAL_BYTES = 2
    REQUEST_FILE_PART_SIZE = 5
    REQUEST_FORMAT_ALL = 7
    REQUEST_ENABLE_TFLASH = 8

    _TYPES_ = {}

    def __init__(self, request):
        super(SessionSetupPacket, self).__init__(self.TYPE_SESSION)
        self.data_size += 4
        self.request = request

    def get_data(self):
        return super(SessionSetupPacket, self).get_data() + \
                self.pack32(self.request)

    @classmethod
    def from_buf(cls, buf):
        request = cls.unpack32(buf[:4])
        return cls._TYPES_[request].from_buf(buf[4:])
ControlPacket.register_type(ControlPacket.TYPE_SESSION, SessionSetupPacket)


class BeginSessionPacket(SessionSetupPacket):
    def __init__(self):
        super(BeginSessionPacket, self).__init__(self.REQUEST_BEGIN_SESSION)

    @classmethod
    def from_buf(cls, buf):
        return cls()
SessionSetupPacket.register_type(SessionSetupPacket.REQUEST_BEGIN_SESSION,
                                 BeginSessionPacket)
