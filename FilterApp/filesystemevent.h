#pragma once
#include <iostream>
#include <string>

enum UEventType : ULONG {
    Unknown1,
    Create,
    Delete ,
    Change,
    Move,
    RealDelete,
    Overwrite
};
struct FileSystemEvent
{

    ULONG Type;
    std::wstring Filename;
    std::wstring OldFilename;
    unsigned long long ProcessId;
};



//struct LogRecord
//{
//    int Length;           // Length of log record.  This Does not include
//    int SequenceNumber;   // space used by other members of RECORD_LIST
//
//    int RecordType;       // The type of log record this is.
//    int Reserved;        // For alignment on IA64
//
//    RecordData Data;
//};
//
//typedef LogRecord *PLOG_RECORD;
//
//struct RecordData
//{
//    unsigned long OriginatingTime;
//    unsigned long CompletionTime;
//
//    EventType EventType;
//
//    int Flags;
//    unsigned long ProcessId;
//};
