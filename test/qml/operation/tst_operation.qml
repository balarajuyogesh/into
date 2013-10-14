/* This file is part of Into. 
 * Copyright (C) Intopii 2013.
 * All rights reserved.
 *
 * Licensees holding a commercial Into license may use this file in
 * accordance with the commercial license agreement. Please see
 * LICENSE.commercial for commercial licensing terms.
 *
 * Alternatively, this file may be used under the terms of the GNU
 * Affero General Public License version 3 as published by the Free
 * Software Foundation. In addition, Intopii gives you special rights
 * to use Into as a part of open source software projects. Please
 * refer to LICENSE.AGPL3 for details.
 */

import QtTest 1.0
import Into 2.0

TestCase
{
  name: "PiiOperation"

  property var operation

  function show(obj)
  {
    for (var i in obj)
      console.log("'" + i + "': " + obj[i]);
  }

  function checkEnums(obj, values)
  {
    for (var i=0; i<values.length; ++i)
      compare(i, obj[values[i]]);
  }

  function test_00_enums()
  {
    var states = [ "Stopped", "Starting", "Running", "Pausing", "Paused", "Stopping", "Interrupted" ];
    var protectionLevels = [ "WriteAlways", "WriteWhenStoppedOrPaused", "WriteWhenStopped", "WriteNotAllowed" ];

    checkEnums(Into.PiiOperation, states);
    checkEnums(Into.PiiOperation, protectionLevels);
  }

  function test_01_create()
  {
    try
      {
        var nil = new Into.PiiOperation;
        fail("Shouldn't be able to instantiate an abstract class.");
      }
    catch (error)
      {}

    Into.PiiEngine.loadPlugin("piiflowcontrol");
    operation = new Into.PiiPisoOperation();
    compare(typeof(operation), "object");
  }

  function test_02_state()
  {
    compare(operation.state(), Into.PiiOperation.Stopped);
  }

  function test_03_inputs_and_outputs()
  {
    console.log(operation.input("input0"));
    compare(operation.socketName(operation.input("input0")), "input0");
    var inputs = operation.inputs();
    var outputs = operation.outputs();
    compare(inputs.length, 2);
    compare(outputs.length, 1);
  }
}
