#include "DataSerializer.h"
#include <cstdio>

namespace jxcorlib::ser
{
    bool FileStream::IsEOF()
    {
        return (bool)feof((FILE*)this->file_);
    }
    int64_t FileStream::get_position() const
    {
        return _ftelli64((FILE*)this->file_);
    }
    void FileStream::set_position(int64_t value)
    {
        _fseeki64((FILE*)this->file_, value, SEEK_SET);
    }
    FileStream::FileStream(std::string_view filename, FileOpenMode mode)
    {
        char cmode[4]{ 0 };
        switch (mode)
        {
        case jxcorlib::ser::FileOpenMode::Read:
            strcpy(cmode, "r");
            break;
        case jxcorlib::ser::FileOpenMode::Write:
            strcpy(cmode, "w");
            break;
        case jxcorlib::ser::FileOpenMode::ReadWrite:
            strcpy(cmode, "r+");
            break;
        case jxcorlib::ser::FileOpenMode::OpenOrCreate:
            strcpy(cmode, "w+");
            break;
        case jxcorlib::ser::FileOpenMode::Append:
            strcpy(cmode, "a+");
            break;
        default:
            break;
        }
        this->file_ = (void*)fopen(filename.data(), cmode);
        this->mode_ = mode;
    }
    int32_t FileStream::WriteByte(uint8_t value)
    {
        return this->WriteBytes(&value, 0, 1);
    }
    int32_t FileStream::WriteBytes(uint8_t* arr, int32_t offset, int32_t count)
    {
        return (int32_t)fwrite(arr + offset, sizeof(uint8_t), count, (FILE*)this->file_);
    }
    int32_t FileStream::ReadByte(uint8_t* out_byte)
    {
        return this->ReadBytes(out_byte, 0, 1);
    }
    int32_t FileStream::ReadBytes(uint8_t* arr, int32_t offset, int32_t count)
    {
        auto len = fread(arr + offset, sizeof(uint8_t), count, (FILE*)this->file_);
        return len == EOF ? -1 : len;
    }
    void FileStream::Flush()
    {
        fflush((FILE*)this->file_);
    }
    FileStream::~FileStream()
    {
        if (this->file_)
        {
            fclose((FILE*)this->file_);
            this->file_ = nullptr;
        }
    }
    FileStream::FileStream(FileStream&& r)
    {
        this->file_ = r.file_;
        r.file_ = nullptr;
    }


    Stream& ReadWriteStream(Stream& stream, bool is_write, uint8_t& out)
    {
        stream.ReadWriteBytes((uint8_t*)(&out), 0, sizeof(out), is_write);
        return stream;
    }
    Stream& ReadWriteStream(Stream& stream, bool is_write, int16_t& out)
    {
        stream.ReadWriteBytes((uint8_t*)(&out), 0, sizeof(out), is_write);
        return stream;
    }
    Stream& ReadWriteStream(Stream& stream, bool is_write, int32_t& out)
    {
        stream.ReadWriteBytes((uint8_t*)(&out), 0, sizeof(out), is_write);
        return stream;
    }
    Stream& ReadWriteStream(Stream& stream, bool is_write, int64_t& out)
    {
        stream.ReadWriteBytes((uint8_t*)(&out), 0, sizeof(out), is_write);
        return stream;
    }
    Stream& ReadWriteStream(Stream& stream, bool is_write, float& out)
    {
        stream.ReadWriteBytes((uint8_t*)(&out), 0, sizeof(out), is_write);
        return stream;
    }
    Stream& ReadWriteStream(Stream& stream, bool is_write, double& out)
    {
        stream.ReadWriteBytes((uint8_t*)(&out), 0, sizeof(out), is_write);
        return stream;
    }
    Stream& ReadWriteStream(Stream& stream, bool is_write, std::string& out)
    {
        int32_t len = out.length();
        ReadWriteStream(stream, is_write, len);
        if (!is_write)
        {
            out.reserve(len);
        }
        stream.ReadWriteBytes((uint8_t*)out.c_str(), 0, len, is_write);
        if (!is_write)
        {
            //no size
            std::string(out.c_str()).swap(out);
        }
        return stream;
    }
}

