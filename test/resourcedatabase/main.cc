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

#include "TestPiiResourceDatabase.h"

#include <PiiUtil.h>
#include <QtTest>

void TestPiiResourceDatabase::initTestCase()
{
  // Claim: Topi is the designer of PiiResourceDatabase.
  int id = db.addStatement("PiiResourceDatabase", "my:designer", "Topi",
                           PiiResourceStatement::ResourceType);
  // Reificiation: I believe the statement is true
  db.addStatement(id, "my:evaluation", "true");

  // Lasse also participated
  id = db.addStatement("PiiResourceDatabase", "my:designer", "Lasse",
                       PiiResourceStatement::ResourceType);
  db.addStatement(id, "my:evaluation", "true");

  // Olli? Not really.
  id = db.addStatement("PiiResourceDatabase", "my:designer", "Olli",
                       PiiResourceStatement::ResourceType);
  db.addStatement(id, "my:evaluation", "false");

  // Add information about us
  db.addStatement("Topi", "my:title", "CTO");
  db.addStatement("Topi", "my:wife", "Anna", PiiResourceStatement::ResourceType);
  db.addStatement("Lasse", "my:title", "Software Engineer");
  db.addStatement("Lasse", "my:wife", "Tuulikki", PiiResourceStatement::ResourceType);
  db.addStatement("Olli", "my:title", "Keisari");
  db.addStatement("Olli", "my:wife", "Johanna", PiiResourceStatement::ResourceType);

  db.addStatement("Topi", "my:kids", QString::number(6));
  db.addStatement("Lasse", "my:kids", QString::number(3));
  db.addStatement("Olli", "my:kids", QString::number(1));
  
  QCOMPARE(db.statementCount(), 15);
}

void TestPiiResourceDatabase::select()
{
  using namespace Pii;
  {
    QList<PiiResourceStatement> lstResult;

    // Find all supposed designers of PiiResourceDatabase
    lstResult = db.select(predicate == "my:designer");
    QCOMPARE(lstResult.size(), 3);
    QCOMPARE(lstResult[0].object(), QString("Topi"));
    QCOMPARE(lstResult[1].object(), QString("Lasse"));
    QCOMPARE(lstResult[2].object(), QString("Olli"));

    // Find Olli's wife
    lstResult = db.select(subject == "Olli" && predicate == "my:wife");
    QCOMPARE(lstResult.size(), 1);
    QCOMPARE(lstResult[0].object(), QString("Johanna"));

    // Find CTO and the guy whose wife is Anna
    lstResult = db.select(object == "CTO" || attribute("my:wife") == "Anna");
    QCOMPARE(lstResult.size(), 2);
    QCOMPARE(lstResult[0].subject(), QString("Topi"));
    QCOMPARE(lstResult[1].subject(), QString("Topi"));
  }
  {
    // Find the number of true statements.
    QList<int> lstResult = db.select(statementId, attribute("my:evaluation") == "true");
    QCOMPARE(lstResult.size(), 2);
    // Find statements about kids    
    lstResult = db.select(resourceStringTo<int>(object), predicate == "my:kids");
    QCOMPARE(lstResult.size(), 3);
    // Find ids of reification statements
    lstResult = db.select(statementId, resourceIdToInt(subject) != -1);
    QCOMPARE(lstResult.size(), 3);
  }
  {
    QList<QString> lstResult;
    // Find all real designers of PiiResourceDatabase
    lstResult = db.select(object,
                          predicate == "my:designer" &&
                          statementId == (db.select(resourceIdToInt(subject), attribute("my:evaluation") == "true")));
    QCOMPARE(lstResult.size(), 2);
    QCOMPARE(lstResult[0], QString("Topi"));
    QCOMPARE(lstResult[1], QString("Lasse"));

    // Find the wife of the guy with more than five kids.
    lstResult = db.select(object,
                          predicate == "my:wife" &&
                          subject == db.select(subject,
                                               predicate == "my:kids" &&
                                               resourceStringTo<int>(object) > 5));
    QCOMPARE(lstResult.size(), 1);
    QCOMPARE(lstResult[0], QString("Anna"));

    // Find the wife of the guy with less than two kids.
    lstResult = db.select(object,
                          predicate == "my:wife" &&
                          subject == db.select(subject,
                                               resourceStringTo<int>(attribute("my:kids")) < 2));

    QCOMPARE(lstResult.size(), 1);
    QCOMPARE(lstResult[0], QString("Johanna"));

    // Find a very specific statement about PiiResourceDatabase
    lstResult = db.select(object,
                          subject == "PiiResourceDatabase" &&
                          !(attribute("my:designer") == "Topi" ||
                            attribute("my:designer") == "Lasse") &&
                          resourceType == PiiResourceStatement::ResourceType);
    QCOMPARE(lstResult.size(), 1);
    QCOMPARE(lstResult[0], QString("Olli"));
   }
}

QTEST_MAIN(TestPiiResourceDatabase)
