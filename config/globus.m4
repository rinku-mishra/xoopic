dnl ######################################################################
dnl
dnl File:	globus.m4
dnl
dnl Purpose:	Determine where the globus tools are.
dnl
dnl Version:	$Id: globus.m4 2165 2007-01-25 20:44:17Z sveta $
dnl
dnl Copyright Tech-X Corporation, 2001.  Redistribution allowed provided
dnl this copyright statement remains intact.
dnl
dnl ######################################################################
if test -n "$GLOBUS_FLAVOR"; then
	GLOBUS_FLAVOR=$GLOBUS_FLAVOR
else
	GLOBUS_FLAVOR=gcc32dbg
fi

if test -n "$GLOBUS_LOCATION"; then
  GLOBUS_INCDIR=$GLOBUS_LOCATION/include/$GLOBUS_FLAVOR
  GLOBUS_LIBDIR=$GLOBUS_LOCATION/lib
  GLOBUS_LIBS="-L$GLOBUS_LIBDIR -lglobus_wsrf_core_tools_$GLOBUS_FLAVOR -lglobus_c_wsrf_resource_$GLOBUS_FLAVOR -lglobus_wsa_bindings_$GLOBUS_FLAVOR -lglobus_ds_bindings_$GLOBUS_FLAVOR -lglobus_wsu_bindings_$GLOBUS_FLAVOR -lglobus_wsp_bindings_$GLOBUS_FLAVOR -lglobus_wst_bindings_$GLOBUS_FLAVOR -lglobus_wsse_bindings_$GLOBUS_FLAVOR -lglobus_wsseu_bindings_$GLOBUS_FLAVOR -lglobus_wsc_bindings_$GLOBUS_FLAVOR -lglobus_wsbf_bindings_$GLOBUS_FLAVOR -lglobus_wsnt_bindings_$GLOBUS_FLAVOR -lglobus_wsrl_bindings_$GLOBUS_FLAVOR -lglobus_wsrp_bindings_$GLOBUS_FLAVOR -lglobus_wssg_bindings_$GLOBUS_FLAVOR -lglobus_notification_consumer_bindings_$GLOBUS_FLAVOR -lglobus_subscription_manager_bindings_$GLOBUS_FLAVOR -lglobus_ws_messaging_$GLOBUS_FLAVOR -lglobus_usage_$GLOBUS_FLAVOR -lglobus_io_$GLOBUS_FLAVOR -lglobus_xio_$GLOBUS_FLAVOR -lgssapi_error_$GLOBUS_FLAVOR -lglobus_gss_assist_$GLOBUS_FLAVOR -lglobus_gssapi_gsi_$GLOBUS_FLAVOR -lglobus_gsi_proxy_core_$GLOBUS_FLAVOR -lglobus_gsi_credential_$GLOBUS_FLAVOR -lglobus_gsi_callback_$GLOBUS_FLAVOR -lglobus_oldgaa_$GLOBUS_FLAVOR -lglobus_gsi_sysconfig_$GLOBUS_FLAVOR -lglobus_gsi_cert_utils_$GLOBUS_FLAVOR -lglobus_openssl_$GLOBUS_FLAVOR -lglobus_openssl_error_$GLOBUS_FLAVOR -lglobus_callout_$GLOBUS_FLAVOR -lglobus_proxy_ssl_$GLOBUS_FLAVOR -lglobus_common_$GLOBUS_FLAVOR -lssl_$GLOBUS_FLAVOR -lcrypto_$GLOBUS_FLAVOR -lxml2_$GLOBUS_FLAVOR -lltdl_$GLOBUS_FLAVOR -lglobus_ftp_client_$GLOBUS_FLAVOR -lglobus_ftp_control_$GLOBUS_FLAVOR"

else
  AC_MSG_WARN(Unable to find Globus.)
fi

if test -z "$GLOBUS_LOCATION"; then
  AC_MSG_WARN(Unable to find Globus.)
else
  AC_DEFINE(HAVE_GLOBUS, , Define if globus is there)
fi

AC_SUBST(GLOBUS_LOCATION)
AC_SUBST(GLOBUS_LIBDIR)
AC_SUBST(GLOBUS_LIBS)
AC_SUBST(GLOBUS_INCDIR)
AC_SUBST(GLOBUS_FLAVOR)
