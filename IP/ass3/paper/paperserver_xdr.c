/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "paperserver.h"

bool_t
xdr_paper (XDR *xdrs, paper *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, &objp->author_name, 64))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->paper_name, 64))
		 return FALSE;
	 if (!xdr_bytes (xdrs, (char **)&objp->data.data_val, (u_int *) &objp->data.data_len, ~0))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_info_out (XDR *xdrs, info_out *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, &objp->author_name, 64))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->paper_name, 64))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_paper_out (XDR *xdrs, paper_out *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->succes))
		 return FALSE;
	 if (!xdr_bytes (xdrs, (char **)&objp->data.data_val, (u_int *) &objp->data.data_len, ~0))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_list_out (XDR *xdrs, list_out *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->id))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->author_name, 64))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->paper_title, 64))
		 return FALSE;
	 if (!xdr_pointer (xdrs, (char **)&objp->next, sizeof (list_out), (xdrproc_t) xdr_list_out))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_add_out (XDR *xdrs, add_out *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, objp))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_remove_in (XDR *xdrs, remove_in *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, objp))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_info_in (XDR *xdrs, info_in *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, objp))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_fetch_in (XDR *xdrs, fetch_in *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, objp))
		 return FALSE;
	return TRUE;
}
