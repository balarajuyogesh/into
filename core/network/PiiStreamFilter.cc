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

#include "PiiStreamFilter.h"

PiiStreamFilter::Data::~Data() {}

PiiStreamFilter::PiiStreamFilter() : d(0) {}
PiiStreamFilter::PiiStreamFilter(Data* data) : d(data) {}
PiiStreamFilter::~PiiStreamFilter() { delete d; }
void PiiStreamFilter::setOutputFilter(PiiStreamFilter*) {}
PiiStreamFilter* PiiStreamFilter::outputFilter() const { return 0; }
qint64 PiiStreamFilter::flushFilter() { return 0; }
qint64 PiiStreamFilter::bufferedSize() const { return -1; }
qint64 PiiStreamFilter::filterData(const QByteArray& array) { return filterData(array.constData(), array.size()); }

PiiDefaultStreamFilter::Data::Data() : pOutputFilter(0) {}
PiiDefaultStreamFilter::PiiDefaultStreamFilter() : PiiStreamFilter(new Data) {}
PiiDefaultStreamFilter::PiiDefaultStreamFilter(Data* data) : PiiStreamFilter(data) {}
void PiiDefaultStreamFilter::setOutputFilter(PiiStreamFilter* outputFilter) { _d()->pOutputFilter = outputFilter; }
PiiStreamFilter* PiiDefaultStreamFilter::outputFilter() const { return _d()->pOutputFilter; }
