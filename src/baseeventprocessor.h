#ifndef BASEEVENTPROCESSOR_H
#define BASEEVENTPROCESSOR_H

namespace skbar {

class Event {

public:
    Event() {}
    virtual ~Event() {}
};


class BaseEventProcessor {

public:
    BaseEventProcessor() {}
    virtual ~BaseEventProcessor() {}

    virtual bool Process(const Event &event) = 0;
};

}
#endif /* BASEEVENTPROCESSOR_H */
