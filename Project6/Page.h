#pragma once
// Ò³Ãæ³éÏóÀà
class Page {
public:
    virtual bool display() const = 0;
    virtual ~Page() {}
};