/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 27 мая 2026 13:27:39
 * Version: 1.0.504
 */

#ifndef _FLCODEC_
#define _FLCODEC_

EXPORT bool flc_decode(const char *ifile, const char *ofile);
EXPORT bool flc_encode(const char *ifile, const char *ofile);

#endif