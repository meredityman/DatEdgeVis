#pragma once
#include "ofMain.h"

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "constants.h"
#include "cnpy.h"

struct WriterSettings {
    std::string  data_root;


    std::string  xpub_ip;
    unsigned int xpub_port = 5554;

    int timeout    = 1000;
    unsigned int buffer_size = 10000;

	WriterSettings(){}
	WriterSettings(std::string xpub_ip, const std::string & data_root):
        data_root(data_root),
        xpub_ip(xpub_ip)
    {}
};


class Writer : public ofThread {

public:

    Writer();
    ~Writer();

    void setup(zmq::context_t & ctx, WriterSettings settings);

    void threadedFunction();

private:

    struct bufferInfo {
        std::string path;
        std::vector<float> data;
        
        bufferInfo(std::string path): path(path){}
        bufferInfo() {}

        void save(){
            std::string npypath = path + ofToString(i++, 3, '0') + ".npy";

            unsigned int rows = data.size() / (DATE_NUM_CHANNELS + 1);
            unsigned int cols = DATE_NUM_CHANNELS + 1;

            if( rows * cols != data.size()){
                ofLogWarning("bufferInfo::save") << "Data corrupt";
            } else {
                cnpy::npy_save( npypath, &(data[0]), { rows, cols}, "w");
            }

            data.clear();
        }

    private:
        unsigned int i = 0;
    };

    bool isSetup = false;    
    std::string session;
    int subsession;
    ofDirectory current_dir;
    WriterSettings settings;
    shared_ptr<zmq::socket_t> sub;

    std::vector<zmq::poller_event<>> events;
    zmq::poller_t<> poller;

    std::map<int, bufferInfo> buffers;

};
