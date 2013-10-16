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
  name: "PiiEngine"

  property PiiEngine jsEngine
  property var trigger
  property var probe

  PiiEngine
  {
    id: qmlEngine
  }

  function show(obj)
  {
    for (var i in obj)
      console.log("'" + i + "': " + obj[i]);
  }

  function initTestCase()
  {
    compare(qmlEngine.childCount(), 0);
    qmlEngine.objectName = "qmlEngine";
    compare(qmlEngine.objectName, "qmlEngine");
    // TODO: QML has no way of specifying default protypes. Thus,
    // functions defined in the prototype object will not be
    // available.
    compare(typeof(qmlEngine.execute), "undefined");
  }

  function test_00_loadPlugin()
  {
    var plugin = Into.PiiEngine.loadPlugin("piibase");
    compare(plugin.resourceName, "PiiBasePlugin");
    compare(plugin.libraryName, "piibase");
    verify(plugin.version.length > 0);
  }

  function test_01_loadPlugins()
  {
    var plugins = Into.PiiEngine.loadPlugins("piiflowcontrol", "piiimage");
    compare(plugins.length, 2);
    compare(plugins[0].resourceName, "PiiFlowControlPlugin");
    compare(plugins[0].libraryName, "piiflowcontrol");
    compare(plugins[1].resourceName, "PiiImagePlugin");
    compare(plugins[1].libraryName, "piiimage");
  }

  function test_02_enums()
  {
    compare(Into.PiiEngine.TextFormat, 0);
    compare(Into.PiiEngine.BinaryFormat, 1);
  }

  function test_03_new_engine()
  {
    jsEngine = new Into.PiiEngine();
    //show(jsEngine);
    compare(jsEngine.childCount(), 0);
  }

  function test_04_empty_start_stop()
  {
    compare(jsEngine.childCount(), 0);
    jsEngine.execute();
    compare(jsEngine.state(), PiiOperation.Running);
    jsEngine.stop();
    compare(jsEngine.state(), PiiOperation.Stopped);
  }

  function test_05_add_operations()
  {
    trigger = new Into.PiiTriggerSource("trigger");
    var sgen1 = new Into.PiiSequenceGenerator("sgen1", { sequenceEnd: 2, step: 2 });
    compare(sgen1.objectName, "sgen1");
    compare(sgen1.sequenceEnd, 2);
    compare(sgen1.step, 2);
    var sgen2 = jsEngine.createOperation("PiiSequenceGenerator", "sgen2");
    compare(sgen2.objectName, "sgen2");
    compare(jsEngine.childCount(), 1);

    var sum = new Into.PiiArithmeticOperation("sum");
    jsEngine.addOperations(trigger, sgen1, sum);
    compare(jsEngine.childCount(), 4);
    compare(jsEngine.childAt(0), trigger);
    compare(jsEngine.childAt(-1), null);
    compare(jsEngine.childAt(4), null);
  }

  function test_06_connect()
  {
    verify(jsEngine.connectOutput("trigger.trigger", "sgen1.trigger"));
    // Alternative way
    jsEngine.output("trigger.trigger").connectInput(jsEngine.input("sgen2.trigger"));
    compare(jsEngine.input("sgen2.trigger").connectedOutput(), jsEngine.output("trigger.trigger"));
    verify(jsEngine.connectOutput("sgen1.output", "sum.input0"));
    verify(jsEngine.connectOutput("sgen2.output", "sum.input1"));
  }

  function test_07_expose_output()
  {
    verify(jsEngine.exposeOutput("sum.output"));
    verify(!jsEngine.exposeOutput("sum.output"));
    verify(jsEngine.createOutputProxy("sequence1", "sgen1.output") != null);
    verify(jsEngine.createOutputProxy("sequence2", "sgen2.output") != null);
    compare(jsEngine.outputCount(), 3);
    jsEngine.removeOutput("sequence2");
    compare(jsEngine.outputCount(), 2);
    jsEngine.removeOutput(jsEngine.output("sequence1"));
    compare(jsEngine.outputCount(), 1);
    jsEngine.removeOutput(jsEngine.output("sum.output"));
    compare(jsEngine.outputCount(), 0);
    jsEngine.exposeOutput("sum.output");
    compare(jsEngine.outputCount(), 1);
  }

  function test_08_add_probe()
  {
    probe = new Into.PiiProbeInput;
    verify(jsEngine.connectOutput("output", probe));
    verify(!probe.hasSavedObject());
  }

  function test_09_execute()
  {
    jsEngine.execute();
    verify(jsEngine.wait(PiiOperation.Running, 1000));
    trigger.trigger();
    verify(probe.hasSavedObject());
    compare(probe.savedObject.toInt(), 0);
    trigger.trigger();
    compare(probe.savedObject.toInt(), 3);
    jsEngine.stop();
    verify(jsEngine.wait(PiiOperation.Stopped, 1000));
  }

  function test_10_save_load()
  {
    jsEngine.save("test.cft");
    var jsEngine2 = Into.PiiEngine.load("test.cft");
    compare(jsEngine2.childCount(), 4);
  }
}
