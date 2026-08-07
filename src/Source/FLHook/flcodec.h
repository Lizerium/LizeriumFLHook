/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 07 августа 2026 08:25:29
 * Version: 1.0.575
 */

#ifndef _FLCODEC_
#define _FLCODEC_

EXPORT bool flc_decode(const char *ifile, const char *ofile);
EXPORT bool flc_encode(const char *ifile, const char *ofile);

#endif