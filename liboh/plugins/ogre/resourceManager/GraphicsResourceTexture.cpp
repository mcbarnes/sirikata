/*  Meru
 *  GraphicsResourceTexture.cpp
 *
 *  Copyright (c) 2009, Stanford University
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
#include "CDNArchive.hpp"
#include "GraphicsResourceManager.hpp"
#include "GraphicsResourceTexture.hpp"
#include "ResourceDependencyTask.hpp"
#include "ResourceLoadTask.hpp"
#include "ResourceLoadingQueue.hpp"
#include "ResourceUnloadTask.hpp"
#include "SequentialWorkQueue.hpp"
#include <boost/bind.hpp>
#include <OgreResourceBackgroundQueue.h>

namespace Meru {

class TextureDependencyTask : public ResourceDependencyTask
{
public:
  TextureDependencyTask(DependencyManager* mgr, WeakResourcePtr resource, const String& hash);
  virtual ~TextureDependencyTask();

  virtual void operator()();
};

class TextureLoadTask : public ResourceLoadTask
{
public:
  TextureLoadTask(DependencyManager *mgr, SharedResourcePtr resource, const String &hash, unsigned int epoch);

  virtual void doRun();

protected:
  unsigned int mArchiveName;
};

class TextureUnloadTask : public ResourceUnloadTask
{
public:
  TextureUnloadTask(DependencyManager *mgr, WeakResourcePtr resource, const String &hash, unsigned int epoch);

  virtual void doRun();

protected:
  //bool mainThreadUnload(String name);
};

GraphicsResourceTexture::GraphicsResourceTexture(const RemoteFileId &resourceID)
  : GraphicsResourceAsset(resourceID, GraphicsResource::TEXTURE)
{

}

GraphicsResourceTexture::~GraphicsResourceTexture()
{
  if (mLoadState == LOAD_LOADED)
    doUnload();
}

ResourceDownloadTask* GraphicsResourceTexture::createDownloadTask(DependencyManager *manager, ResourceRequestor *resourceRequestor)
{
  return new ResourceDownloadTask(manager, mResourceID, resourceRequestor);
}

ResourceDependencyTask* GraphicsResourceTexture::createDependencyTask(DependencyManager *manager)
{
  return new TextureDependencyTask(manager, getWeakPtr(), mResourceID.toString());
}

ResourceLoadTask* GraphicsResourceTexture::createLoadTask(DependencyManager *manager)
{
  return new TextureLoadTask(manager, getSharedPtr(), mResourceID.toString(), mLoadEpoch);
}

ResourceUnloadTask* GraphicsResourceTexture::createUnloadTask(DependencyManager *manager)
{
  return new TextureUnloadTask(manager, getWeakPtr(), mResourceID.toString(), mLoadEpoch);
}

/***************************** TEXTURE DEPENDENCY TASK *************************/

TextureDependencyTask::TextureDependencyTask(DependencyManager *mgr, WeakResourcePtr resource, const String& hash)
  : ResourceDependencyTask(mgr, resource, hash)
{

}

TextureDependencyTask::~TextureDependencyTask()
{

}

void TextureDependencyTask::operator()()
{
  SharedResourcePtr resourcePtr = mResource.lock();
  if (!resourcePtr) {
    finish(false);
    return;
  }

  resourcePtr->setCost(mBuffer.size());
  resourcePtr->parsed(true);

  finish(true);
}

/***************************** TEXTURE LOAD TASK *************************/

TextureLoadTask::TextureLoadTask(DependencyManager *mgr, SharedResourcePtr resourcePtr, const String &hash, unsigned int epoch)
: ResourceLoadTask(mgr, resourcePtr, hash, epoch)
{
}

void TextureLoadTask::doRun()
{
  mArchiveName = CDNArchive::addArchive(CDNArchive::canonicalMhashName(mHash), mBuffer);
  Ogre::TextureManager::getSingleton().load(CDNArchive::canonicalMhashName(mHash), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  CDNArchive::removeArchive(mArchiveName);
  mResource->loaded(true, mEpoch);
}

/***************************** TEXTURE UNLOAD TASK *************************/

TextureUnloadTask::TextureUnloadTask(DependencyManager *mgr, WeakResourcePtr resource, const String &hash, unsigned int epoch)
: ResourceUnloadTask(mgr, resource, hash, epoch)
{

}
/*
bool TextureUnloadTask::mainThreadUnload(String name)
{
  Ogre::TextureManager::getSingleton().unload(name);
  operationCompleted(Ogre::BackgroundProcessTicket(), Ogre::BackgroundProcessResult());
  return false;
}
*/
void TextureUnloadTask::doRun()
{
  /*I REALLY wish this were true*/
  //    SequentialWorkQueue::getSingleton().queueWork(std::tr1::bind(&TextureUnloadTask::mainThreadUnload, this, mHash));

  Ogre::TextureManager* textureManager = Ogre::TextureManager::getSingletonPtr();
  textureManager->remove(mHash);

  Ogre::ResourcePtr textureResource = textureManager->getByName(mHash);
  assert(textureResource.isNull());

  SharedResourcePtr resource = mResource.lock();
  if (resource)
    resource->unloaded(true, mEpoch);
}

}
