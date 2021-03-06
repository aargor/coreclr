// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
// 

// 


#ifndef __SECURITYDESCRIPTOR_APPDOMAIN_H__
#define __SECURITYDESCRIPTOR_APPDOMAIN_H__
#include "security.h"
#include "securitydescriptor.h"
#include "securitymeta.h"

///////////////////////////////////////////////////////////////////////////////
//
//      [SecurityDescriptor]
//      |
//      +----[PEFileSecurityDescriptor]
//      |
//      +----[ApplicationSecurityDescriptor]
//      |
//      +----[AssemblySecurityDescriptor]
//
//      [SharedSecurityDescriptor]
//
///////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------
//
//          APPDOMAIN SECURITY DESCRIPTOR
//
//------------------------------------------------------------------

class ApplicationSecurityDescriptor : public SecurityDescriptorBase<IApplicationSecurityDescriptor>
{
public:
    VPTR_VTABLE_CLASS(ApplicationSecurityDescriptor, SecurityDescriptorBase<IApplicationSecurityDescriptor>)

private:
    // Dependency in managed : System.Security.HostSecurityManager.cs
    enum HostSecurityManagerFlags
    {
        // Flags to control which HostSecurityManager features are provided by the host
        HOST_NONE                   = 0x0000,
        HOST_APP_DOMAIN_EVIDENCE    = 0x0001,
        HOST_POLICY_LEVEL           = 0x0002,
        HOST_ASM_EVIDENCE           = 0x0004,
        HOST_DAT                    = 0x0008,
        HOST_RESOLVE_POLICY         = 0x0010
    };


    // The bits represent the status of security checks on some specific permissions within this domain
    Volatile<DWORD> m_dwDomainWideSpecialFlags;
    // m_dwDomainWideSpecialFlags bit map
    // Bit 0 = Unmanaged Code access permission. Accessed via SECURITY_UNMANAGED_CODE
    // Bit 1 = Skip verification permission. SECURITY_SKIP_VER
    // Bit 2 = Permission to Reflect over types. REFLECTION_TYPE_INFO
    // Bit 3 = Permission to Assert. SECURITY_ASSERT
    // Bit 4 = Permission to invoke methods. REFLECTION_MEMBER_ACCESS
    // Bit 7 = PermissionSet, fulltrust SECURITY_FULL_TRUST
    // Bit 9 = UIPermission (unrestricted)

    BOOL m_fIsInitializationInProgress; // appdomain is in the initialization stage and is considered FullTrust by the security system.
    BOOL m_fIsDefaultAppdomain;         // appdomain is the default appdomain, or created by the default appdomain without an explicit evidence
    BOOL m_fIsDefaultAppdomainEvidence; // Evidence for this AD is the same as the Default AD. 
                                        // m_ifIsDefaultAppDomain is TRUE => m_fIsDefaultAppdomainEvidence is TRUE
                                        // m_fIsDefaultAppdomainEvidence can be TRUE when m_fIsDefaultAppdomain is FALSE if a homogeneous AD was
                                        // created without evidence (non-null PermissionSet though). 
                                        // m_fIsDefaultAppdomainEvidence and m_fIsDefaultAppdomain are both FALSE when an explicit evidence
                                        // exists on the AppDomain. (In the managed world: AppDomain._SecurityIdentity != null)
    BOOL m_fHomogeneous;                // This AppDomain has an ApplicationTrust
    BOOL m_fRuntimeSuppliedHomogenousGrantSet; // This AppDomain is homogenous only because the v4 CLR defaults to creating homogenous domains, and would not have been homogenous in v2
    DWORD m_dwHostSecurityManagerFlags; // Flags indicating what decisions the host wants to participate in.
    BOOL m_fContainsAnyRefusedPermissions;

    BOOL m_fIsPreResolved;              // Have we done a pre-resolve on this domain yet
    BOOL m_fPreResolutionFullTrust;     // Was the domain pre-resolved to be full trust
    BOOL m_fPreResolutionHomogeneous;   // Was the domain pre-resolved to be homogenous
    

#ifndef DACCESS_COMPILE
public:
    //--------------------
    // Constructor
    //--------------------
    inline ApplicationSecurityDescriptor(AppDomain *pAppDomain);

    //--------------------
    // Destructor
    //--------------------

public:
    // Indicates whether the initialization phase is in progress.
    virtual BOOL IsInitializationInProgress();
    inline void ResetInitializationInProgress();

    // The AppDomain is considered a default one (FT) if the property is
    // set and it's not a homogeneous AppDomain (ClickOnce case for example).
    virtual BOOL IsDefaultAppDomain() const;
    inline void SetDefaultAppDomain();

    virtual BOOL IsDefaultAppDomainEvidence();
    inline void SetDefaultAppDomainEvidence();

    virtual VOID Resolve();

    void ResolveWorker();

    virtual void FinishInitialization();

    virtual void PreResolve(BOOL *pfIsFullyTrusted, BOOL *pfIsHomogeneous);

    virtual void SetHostSecurityManagerFlags(DWORD dwFlags);
    virtual void SetPolicyLevelFlag();

    inline void SetHomogeneousFlag(BOOL fRuntimeSuppliedHomogenousGrantSet);
    virtual BOOL IsHomogeneous() const;

    
    virtual BOOL ContainsAnyRefusedPermissions();

    // Should the HSM be consulted for security decisions in this AppDomain.
    virtual BOOL CallHostSecurityManager();


    // Initialize the PLS on the AppDomain.
    void InitializePLS();

    // Called everytime an AssemblySecurityDescriptor is resolved.
    void AddNewSecDescToPLS(AssemblySecurityDescriptor *pNewSecDescriptor);


    // Checks for one of the special domain wide flags 
    // such as if we are currently in a "fully trusted" environment
    // or if unmanaged code access is allowed at this time
    inline BOOL CheckDomainWideSpecialFlag(DWORD flags) const;
    virtual DWORD GetDomainWideSpecialFlag() const;


    virtual BOOL DomainMayContainPartialTrustCode();

    BOOL QuickIsFullyTrusted();

#endif // #ifndef DACCESS_COMPILE
};

#include "securitydescriptorappdomain.inl"

#endif // #define __SECURITYDESCRIPTOR_APPDOMAIN_H__
