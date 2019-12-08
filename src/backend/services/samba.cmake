set (SAMBA_SRC ${DEPS_DIR}/samba-4.10.6)
set (SMB_INCLUDES
        ${SAMBA_SRC}
        ${SAMBA_SRC}/include
        ${SAMBA_SRC}/param
        ${SAMBA_SRC}/ntvfs
        ${SAMBA_SRC}/lib
        ${SAMBA_SRC}/lib/crypto
        ${SAMBA_SRC}/lib/replace
        ${SAMBA_SRC}/lib/talloc
        ${SAMBA_SRC}/lib/appweb/ejs
        ${SAMBA_SRC}/lib/tdb/include
        ${SAMBA_SRC}/librpc/ndr
        ${SAMBA_SRC}/bin/default
        ${SAMBA_SRC}/bin/default/librpc/gen_ndr
        ${SAMBA_SRC}/source4
        ${SAMBA_SRC}/source4/include
        ${SAMBA_SRC}/bin/default/include
        ${SAMBA_SRC}/bin/default/include/public
        ${SAMBA_SRC}/bin/default/source4
        )
set (SMB_LINKS
        ${SAMBA_SRC}/bin/shared/
        ${SAMBA_SRC}/bin/shared/private/
        ${SAMBA_SRC}/bin/default/libcli/util
        ${SAMBA_SRC}/bin/default/lib/util
        ${SAMBA_SRC}/bin/default/lib/param
        ${SAMBA_SRC}/bin/default/lib/tevent
        ${SAMBA_SRC}/bin/default/lib/talloc
        ${SAMBA_SRC}/bin/default/auth/credentials
        ${SAMBA_SRC}/bin/default/source4/lib/registry
        ${SAMBA_SRC}/bin/default/lib/auth/credentials
        ${SAMBA_SRC}/bin/default/source3/libsmb

        )
link_directories(${SMB_LINKS})
include_directories(${SMB_INCLUDES})