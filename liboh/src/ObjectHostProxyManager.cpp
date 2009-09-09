/*  Sirikata liboh -- Object Host
 *  ObjectHostProxyManager.cpp
 *
 *  Copyright (c) 2009, Patrick Reiter Horn
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Sirikata nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <oh/Platform.hpp>
#include <util/SpaceObjectReference.hpp>
#include "oh/ObjectHostProxyManager.hpp"
#include "oh/ObjectHost.hpp"
#include "oh/HostedObject.hpp"
namespace Sirikata {

void ObjectHostProxyManager::initialize() {
}
void ObjectHostProxyManager::destroy() {
    for (ProxyMap::iterator iter = mProxyMap.begin();
         iter != mProxyMap.end();
         ++iter) {
        iter->second.obj->destroy();
    }
    mProxyMap.clear();
}
ObjectHostProxyManager::~ObjectHostProxyManager() {
	destroy();
}

void ObjectHostProxyManager::createViewedObject(const ProxyObjectPtr &newObj, QueryTracker*viewer) {
    std::pair<ProxyMap::iterator, bool> result = mProxyMap.insert(
        ProxyMap::value_type(newObj->getObjectReference().object(), newObj));
    if (result.second==true) {
        notify(&ProxyCreationListener::onCreateProxy,newObj);
    }
    result.first->second.viewers.insert(viewer);
}
void ObjectHostProxyManager::destroyViewedObject(const SpaceObjectReference &newObj, QueryTracker*viewer) {
    ProxyMap::iterator iter = mProxyMap.find(newObj.object());
    if (iter != mProxyMap.end()) {
        std::tr1::unordered_multiset<QueryTracker*>::iterator viewiter;
        viewiter = iter->second.viewers.find(viewer);
        iter->second.viewers.erase(viewiter);
        viewiter = iter->second.viewers.find(viewer);
        if (viewiter == iter->second.viewers.end()) {
            iter->second.obj->destroy();
            notify(&ProxyCreationListener::onDestroyProxy,iter->second.obj);
            mProxyMap.erase(iter);
        }
    }
}
void ObjectHostProxyManager::createObject(const ProxyObjectPtr &newObj) {
    createViewedObject(newObj, 0);
}
void ObjectHostProxyManager::destroyObject(const ProxyObjectPtr &newObj) {
    destroyViewedObject(newObj->getObjectReference(), 0);
}

QueryTracker *ObjectHostProxyManager::getQueryTracker(const SpaceObjectReference &id) const {
    ProxyMap::const_iterator iter = mProxyMap.find(id.object());  
    if (iter == mProxyMap.end()){
        return 0;
    }
    return *(iter->second.viewers.begin());
}

ProxyObjectPtr ObjectHostProxyManager::getProxyObject(const SpaceObjectReference &id) const {
    if (id.space() == mSpaceID) {
        ProxyMap::const_iterator iter = mProxyMap.find(id.object());
        if (iter != mProxyMap.end()) {
            return (*iter).second.obj;
        }
    }
    return ProxyObjectPtr();
}

}
