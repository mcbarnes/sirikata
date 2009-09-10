/*  Sirikata liboh -- Collada Models System
 *  ColladaSystem.hpp
 *
 *  Copyright (c) 2009, Mark C. Barnes
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

#ifndef _SIRIKATA_COLLADA_SYSTEM_
#define _SIRIKATA_COLLADA_SYSTEM_

#include <oh/Platform.hpp>
#include <oh/ModelsSystem.hpp>
#include <util/ListenerProvider.hpp>

//#include <task/EventManager.hpp>

namespace Sirikata { namespace Models {

class SIRIKATA_PLUGIN_EXPORT ColladaSystem
    :   public ModelsSystem
{
    public:
        ColladaSystem ();
        virtual ~ColladaSystem ();

        static ColladaSystem* create ( Provider< ProxyCreationListener* >* proxyManager, String const& options );
    
    protected:

    private:
        ColladaSystem ( ColladaSystem const& ); // not implemented
        ColladaSystem& operator = ( ColladaSystem const & ); // not implemented

        bool initialize ( Provider< ProxyCreationListener* >* proxyManager, String const& options );

    // interface from ModelsSystem
    public:
    protected:
    
    // interface from ProxyCreationListener
    public:
        virtual void onCreateProxy ( ProxyObjectPtr object );
        virtual void onDestroyProxy ( ProxyObjectPtr object );

    protected:

};

} // namespace Models
} // namespace Sirikata

#endif // _SIRIKATA_COLLADA_SYSTEM_
