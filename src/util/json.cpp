#include "json.h"
#include "timing.h"

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>

using namespace rapidjson;

namespace util {
    std::string messageToJson(const message::Message &msg) {
        Document d;
        d.SetObject();
        Document::AllocatorType &a = d.GetAllocator();

        // Need to be explicit with strings here to make a copy _and_ make sure we specify the length
        // to include any null-terminators from bytes
        d.AddMember("id", msg.id(), a);
        d.AddMember("user", Value(msg.user().c_str(), msg.user().size(), a).Move(), a);
        d.AddMember("function", Value(msg.function().c_str(), msg.function().size(), a).Move(), a);
        d.AddMember("index", msg.idx(), a);
        d.AddMember("hops", msg.hops(), a);

        d.AddMember("snapshot_key", Value(msg.snapshotkey().c_str(), msg.snapshotkey().size(), a).Move(), a);
        d.AddMember("snapshot_size", msg.snapshotsize(), a);
        d.AddMember("func_ptr", msg.funcptr(), a);

        d.AddMember("py_user", Value(msg.pythonuser().c_str(), msg.pythonuser().size(), a).Move(), a);
        d.AddMember("py_func", Value(msg.pythonfunction().c_str(), msg.pythonfunction().size(), a).Move(), a);
        d.AddMember("py_entry", Value(msg.pythonentry().c_str(), msg.pythonentry().size(), a).Move(), a);

        d.AddMember("input_data", Value(msg.inputdata().c_str(), msg.inputdata().size(), a).Move(), a);
        d.AddMember("output_data", Value(msg.outputdata().c_str(), msg.outputdata().size(), a).Move(), a);

        d.AddMember("async", msg.isasync(), a);
        d.AddMember("python", msg.ispython(), a);
        d.AddMember("typescript", msg.istypescript(), a);
        d.AddMember("status", msg.isstatusrequest(), a);
        d.AddMember("flush", msg.isflushrequest(), a);

        d.AddMember("result_key", Value(msg.resultkey().c_str(), msg.resultkey().size()).Move(), a);
        d.AddMember("status_key", Value(msg.statuskey().c_str(), msg.statuskey().size()).Move(), a);

        d.AddMember("cold_start_interval", msg.coldstartinterval(), a);

        d.AddMember("mpi", msg.ismpi(), a);
        d.AddMember("mpi_world_id", msg.mpiworldid(), a);
        d.AddMember("mpi_rank", msg.mpirank(), a);
        d.AddMember("mpi_world_size", msg.mpiworldsize(), a);

        d.AddMember("cmdline", Value(msg.cmdline().c_str(), msg.cmdline().size()).Move(), a);

        StringBuffer sb;
        Writer<StringBuffer> writer(sb);
        d.Accept(writer);

        return sb.GetString();
    }

    bool getBoolFromJson(Document &doc, const std::string &key, bool dflt) {
        Value::MemberIterator it = doc.FindMember(key.c_str());
        if (it == doc.MemberEnd()) {
            return dflt;
        }

        return it->value.GetBool();
    }

    int getIntFromJson(Document &doc, const std::string &key, int dflt) {
        Value::MemberIterator it = doc.FindMember(key.c_str());
        if (it == doc.MemberEnd()) {
            return dflt;
        }

        return it->value.GetInt();
    }

    std::string getStringFromJson(Document &doc, const std::string &key, const std::string &dflt) {
        Value::MemberIterator it = doc.FindMember(key.c_str());
        if (it == doc.MemberEnd()) {
            return dflt;
        }

        const char *valuePtr = it->value.GetString();
        return std::string(valuePtr, valuePtr + it->value.GetStringLength());
    }

    message::Message jsonToMessage(const std::string &jsonIn) {
        PROF_START(jsonDecode)

        MemoryStream ms(jsonIn.c_str(), jsonIn.size());
        Document d;
        d.ParseStream(ms);

        message::Message msg;
        msg.set_id(getIntFromJson(d, "id", 0));
        msg.set_user(getStringFromJson(d, "user", ""));
        msg.set_function(getStringFromJson(d, "function", ""));
        msg.set_idx(getIntFromJson(d, "index", 0));
        msg.set_hops(getIntFromJson(d, "hops", 0));

        msg.set_snapshotkey(getStringFromJson(d, "snapshot_key", ""));
        msg.set_snapshotsize(getIntFromJson(d, "snapshot_size", 0));
        msg.set_funcptr(getIntFromJson(d, "func_ptr", 0));

        msg.set_pythonuser(getStringFromJson(d, "py_user", ""));
        msg.set_pythonfunction(getStringFromJson(d, "py_func", ""));
        msg.set_pythonentry(getStringFromJson(d, "py_entry", ""));

        msg.set_inputdata(getStringFromJson(d, "input_data", ""));
        msg.set_outputdata(getStringFromJson(d, "output_data", ""));

        msg.set_isasync(getBoolFromJson(d, "async", false));
        msg.set_ispython(getBoolFromJson(d, "python", false));
        msg.set_istypescript(getBoolFromJson(d, "typescript", false));
        msg.set_isstatusrequest(getBoolFromJson(d, "status", false));
        msg.set_isflushrequest(getBoolFromJson(d, "flush", false));

        msg.set_resultkey(getStringFromJson(d, "result_key", ""));
        msg.set_statuskey(getStringFromJson(d, "status_key", ""));

        msg.set_coldstartinterval(getIntFromJson(d, "cold_start_interval", 0));

        msg.set_type(message::Message_MessageType_CALL);

        msg.set_ismpi(getBoolFromJson(d, "mpi", false));
        msg.set_mpiworldid(getIntFromJson(d, "mpi_world_id", -1));
        msg.set_mpirank(getIntFromJson(d, "mpi_rank", -1));
        msg.set_mpiworldsize(getIntFromJson(d, "mpi_world_size", -1));

        msg.set_cmdline(getStringFromJson(d, "cmdline", ""));

        PROF_END(jsonDecode)

        return msg;
    }
    
    std::string getValueFromJsonString(const std::string &key, const std::string &jsonIn) {
        MemoryStream ms(jsonIn.c_str(), jsonIn.size());
        Document d;
        d.ParseStream(ms);

        std::string result = getStringFromJson(d, key, "");
        return result;
    }
}

