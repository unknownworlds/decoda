
#include <vector>
#include <wx/process.h>

#include "ProcessOutputSink.h"
#include "OutputWindow.h"

#include <algorithm>

#ifndef LENGTH_OF
    #define LENGTH_OF(a) (sizeof(a) / sizeof(*a))
#endif

struct ProcessOutputSink::Sink
{
    struct Buffer {
        Buffer() : _head(NULL), _tail(NULL) { }
        wxChar* _head; wxChar* _tail;
        wxChar _data[ProcessOutputSink::kPumpBatchSize];
    } _message, _error;
};

namespace
{
    void DumpSink(OutputWindow& output, ProcessOutputSink::Sink::Buffer& buffer, bool requireEOL)
    {
        bool    hasLine = false;
        wxChar* offset  = buffer._head;
        while (offset < buffer._tail)
        {
            wxChar* eol    = wxStrchr(offset, wxT('\n'));
            size_t  length = size_t(buffer._tail - offset);
            if (eol)
            {
                hasLine = true;
                length  = size_t(eol - offset);
            }
            else if (hasLine && requireEOL)
                break;
            
            if (length > 0)
            {
                wxString s(offset, length);
                output.OutputMessage(s);
            }
            
            offset += length + 1;
        }
        
        buffer._head = std::min(offset, buffer._tail);
    }
    
    void PumpStream(OutputWindow& output, wxInputStream& stream, ProcessOutputSink::Sink::Buffer& buffer)
    {
        if (!stream.CanRead()) return;
        
        // Move any data remaining to head of buffer. Nop if remainder==0.
        assert(buffer._head <= buffer._tail);
        size_t remainder = size_t(buffer._tail - buffer._head);
        memmove(buffer._data, buffer._head, remainder * sizeof(wxChar));
        
        // Fill in rest of buffer
        size_t size = sizeof(buffer._data) - remainder * sizeof(wxChar);
        stream.Read(buffer._data + remainder, size);
        buffer._head = buffer._data;
        buffer._tail = buffer._data + remainder + stream.LastRead();
        
        DumpSink(output, buffer, true);
    }
}

bool ProcessOutputSink::Pump(OutputWindow& output, wxProcess& process)
{
    bool  hasInput = false;
    Sink* sink     = SinkGet(process);
    
    if (process.IsInputAvailable())
    {
        hasInput = true;
        PumpStream(output, *process.GetInputStream(), sink->_message);
    }
    
    if (process.IsErrorAvailable())
    {
        hasInput = true;
        PumpStream(output, *process.GetErrorStream(), sink->_error);
    }
    
    return hasInput;
}

void ProcessOutputSink::Dump(OutputWindow& output, wxProcess& process)
{
    while (Pump(output, process));
    
    Sink* sink = SinkGet(process);
    DumpSink(output, sink->_message, false);
    DumpSink(output, sink->_error  , false);
    
    _sinks.erase(_sinks.find(&process));
    delete sink;
}

ProcessOutputSink::Sink* ProcessOutputSink::SinkGet(wxProcess& process)
{
    std::map<wxProcess*, Sink*>::iterator iter = _sinks.find(&process);
    if (iter != _sinks.end()) return iter->second;
    
    Sink* p_sink = new Sink;
    _sinks.insert(std::make_pair(&process, p_sink));
    return p_sink;
}
