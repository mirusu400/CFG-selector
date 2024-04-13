//===- svf-ex.cpp -- A driver example of SVF-------------------------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013->  <Yulei Sui>
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===-----------------------------------------------------------------------===//

/*
 // A driver program of SVF including usages of SVF APIs
 //
 // Author: Yulei Sui,
 */

#include "SVF-LLVM/LLVMUtil.h"
#include "Graphs/SVFG.h"
#include "WPA/Andersen.h"
#include "SVF-LLVM/SVFIRBuilder.h"
#include "Util/Options.h"

using namespace llvm;
using namespace std;
using namespace SVF;



Set<const ICFGNode*> visited;
/*!
 * An example to query/collect all successor nodes from a ICFGNode (iNode) along control-flow graph (ICFG)
 */
void traverseOnICFG(ICFG* icfg, const SVFInstruction* svfinst)
{
    // SVFInstruction* svfinst = LLVMModuleSet::getLLVMModuleSet()->getSVFInstruction(inst);

    ICFGNode* iNode = icfg->getICFGNode(svfinst);
    FIFOWorkList<const ICFGNode*> worklist;
    
    worklist.push(iNode);

    /// Traverse along VFG
    while (!worklist.empty())
    {
        const ICFGNode* iNode = worklist.pop();
        for (ICFGNode::const_iterator it = iNode->OutEdgeBegin(), eit =
                    iNode->OutEdgeEnd(); it != eit; ++it)
        {
            ICFGEdge* edge = *it;
            ICFGNode* succNode = edge->getDstNode();
            if (visited.find(succNode) == visited.end())
            {
                visited.insert(succNode);
                worklist.push(succNode);
            }
        }
    }
}

/*!
 * An example to query/collect all the uses of a definition of a value along value-flow graph (VFG)
 */
int main(int argc, char ** argv)
{
    llvm::LLVMContext context;
    llvm::SMDiagnostic error;
    Instruction* value = nullptr;

    std::vector<std::string> moduleNameVec;
    moduleNameVec = OptionBase::parseOptions(
            argc, argv, "Whole Program Points-to Analysis", "[options] <input-bitcode...>"
    );


    if (Options::WriteAnder() == "ir_annotator")
    {
        LLVMModuleSet::preProcessBCs(moduleNameVec);
    }

    SVFModule* svfModule = LLVMModuleSet::buildSVFModule(moduleNameVec);
    const SVFFunction* svfFunc = svfModule->getSVFFunction("main");
    const SVFBasicBlock *svfBlock = svfFunc->getEntryBlock();
    const SVFInstruction *svfInst = svfBlock->front();
    
    /// Build Program Assignment Graph (SVFIR)
    SVFIRBuilder builder(svfModule);
    SVFIR* pag = builder.build();

    /// Create Andersen's pointer analysis
    Andersen* ander = AndersenWaveDiff::createAndersenWaveDiff(pag);

    /// Query aliases
    /// aliasQuery(ander,value1,value2);

    /// Print points-to information
    /// printPts(ander, value1);

    /// Call Graph
    PTACallGraph* callgraph = ander->getPTACallGraph();

    /// ICFG
    ICFG* icfg = pag->getICFG();



    /// Value-Flow Graph (VFG)
    VFG* vfg = new VFG(callgraph);

    /// Sparse value-flow graph (SVFG)
    SVFGBuilder svfBuilder;
    SVFG* svfg = svfBuilder.buildFullSVFG(ander);
    // Instruction* value = nullptr;

    /// Collect uses of an LLVM Value
    // traverseOnVFG(svfg, value);

    /// Collect all successor nodes on ICFG
    // value = 
    traverseOnICFG(icfg, svfInst);

    // Remove nodes from the ICFG
    // for (const std::pair<const unsigned int, SVF::ICFGNode*>& node : *icfg) {
    //      ICFGNode* icfgNode = node.second;
    //     std::cout << icfgNode << std::endl;
    //     if (visited.find(icfgNode) == visited.end()) {
    //         std::cout << "Removing node: " << icfgNode << std::endl;
    //         // Remove all edges
    //         // This got ERRORS!!!
    //         for (ICFGNode::const_iterator iter = icfgNode->OutEdgeBegin();
    //             iter != icfgNode->OutEdgeEnd(); ++iter)
    //         {
    //             SVF::ICFGEdge* edge = *iter;
    //             icfg->removeICFGEdge(edge);
    //         }

    //         icfg->removeICFGNode(icfgNode);
    //     }
    // }

    // // 3. Generate a dot file from the new ICFG
    // icfg->dump("new_icfg.dot");

    // save the visited nodes into file
    std::ofstream file("visited_nodes.txt");
    for (const ICFGNode* node : visited) {
        std::cout << &node << "\t";
        std::cout << node << "\t";
        std::cout << node->getFun() << "\t";
        std::cout << node->getBB() << "\t";
        std::cout << std::endl;
        file << node << std::endl;
    }
    // Print length of the path from the entry node to the exit node
    std::cout << "Length of the path from the entry node to the exit node: " << visited.size() << std::endl;
    icfg->dump("new_icfg.dot");
    // clean up memory
    // delete vfg;
    AndersenWaveDiff::releaseAndersenWaveDiff();
    SVFIR::releaseSVFIR();



    // svfModule->dumpModulesToFile(".svf.bc");
    LLVMModuleSet::getLLVMModuleSet()->dumpModulesToFile(".svf.bc");
    SVF::LLVMModuleSet::releaseLLVMModuleSet();

    llvm::llvm_shutdown();
    return 0;
}

