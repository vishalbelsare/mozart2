// Copyright © 2011, Université catholique de Louvain
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// *  Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// *  Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "mozart.hh"

namespace mozart {

//////////////////////
// PropertyRegistry //
//////////////////////

void PropertyRegistry::initialize(VM vm) {
  initConfig(vm);
  registerPredefined(vm);
}

void PropertyRegistry::initConfig(VM vm) {
  // Print

  config.printDepth = 10;
  config.printWidth = 20;

  // Errors

  config.defaultExceptionHandler = new (vm) StableNode;
  config.defaultExceptionHandler->init(vm, buildNil(vm));

  config.errorsDebug = true;
  config.errorsDepth = 10;
  config.errorsWidth = 20;
  config.errorsThread = 40;
}

void PropertyRegistry::registerPredefined(VM vm) {
  // Threads

  registerReadOnlyProp<nativeint>(vm, "threads.runnable",
    [] (VM vm) -> nativeint {
      return vm->getThreadPool().getRunnableCount();
    });

  // Print

  registerReadWriteProp(vm, "print.depth", config.printDepth);
  registerReadWriteProp(vm, "print.width", config.printWidth);

  // Errors

  registerProp(vm, "errors.handler",
    [this] (VM vm) -> UnstableNode {
      return { vm, *config.defaultExceptionHandler };
    },
    [this] (VM vm, RichNode value) {
      config.defaultExceptionHandler = value.getStableRef(vm);
    }
  );

  registerReadWriteProp(vm, "errors.debug", config.errorsDebug);
  registerReadWriteProp(vm, "errors.depth", config.errorsDepth);
  registerReadWriteProp(vm, "errors.width", config.errorsWidth);
  registerReadWriteProp(vm, "errors.thread", config.errorsThread);

  // Garbage collection

  registerConstantProp(vm, "gc.watcher",
                       ReadOnlyVariable::build(vm));

  // Limits

  registerConstantProp(vm, "limits.int.min",
                       std::numeric_limits<nativeint>::min());
  registerConstantProp(vm, "limits.int.max",
                       std::numeric_limits<nativeint>::max());
  registerConstantProp(vm, "limits.bytecode.xregisters",
                       std::numeric_limits<ByteCode>::max());

  // Config - generated by CMake

#include "properties-config.cc"
}

}
