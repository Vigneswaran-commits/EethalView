#ifndef CNASTRANOP2NODE_H
#define CNASTRANOP2NODE_H
#include "cnode.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class CNastranOp2Node
{
public:
    CNastranOp2Node();
    static void ScanOp2Record(std::string strHeaderName,int iTableLength,std::vector<int> &ivecBlock,Cnode& oNodeData);
    ~CNastranOp2Node();
protected:
private:
};

#endif // CNASTRANOP2NODE_H
