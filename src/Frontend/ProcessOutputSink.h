/*

Decoda
Copyright (C) 2007-2013 Unknown Worlds Entertainment, Inc. 

This file is part of Decoda.

Decoda is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Decoda is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Decoda.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef PROCESS_OUTPUT_SINK_H
#define PROCESS_OUTPUT_SINK_H

#include <map>

class OutputWindow;
class wxProcess;

class ProcessOutputSink
{
    public:
        /// Amount of bytes to attempt to read from error/output per pump.
        /// This is required since we cannot assume the process will ever
        /// emit an EOL, potentially deadlocking us if it emits faster than
        /// we may read. This also allows us to perform some minimal amount of
        /// batch/grouping to prevent unnecessary interweaving of stdout/stderr.
        /// 
        /// If we've > 0 EOL in the batch, then we save the left overs (if any)
        /// for the next pump and dump it then. This makes the output a bit
        /// prettier if we hit the batch-limit mid-line.
        static const size_t kPumpBatchSize = 2048;
        struct Sink;
        
        bool Pump(OutputWindow& output, wxProcess& process);
        void Dump(OutputWindow& output, wxProcess& process);
        
    private:
        Sink* SinkGet(wxProcess& process);
        std::map<wxProcess*, Sink*> _sinks;
};

#endif

