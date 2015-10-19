#ifndef SACFORMAT_H
#define SACFORMAT_H

#include <QFile>
#include <QString>
#include <QDateTime>

typedef qint32 int32;

/* SAC header structure as it exists in binary SAC files */
struct SACHeader
{
	float	delta;			/* RF time increment, sec    */
	float	depmin;			/*    minimum amplitude      */
	float	depmax;			/*    maximum amplitude      */
	float	scale;			/*    amplitude scale factor */
	float	odelta;			/*    observed time inc      */
	float	b;			/* RD initial value, time    */
	float	e;			/* RD final value, time      */
	float	o;			/*    event start, sec < nz. */
	float	a;			/*    1st arrival time       */
	float	fmt;			/*    internal use           */
	float	t0;			/*    user-defined time pick */
	float	t1;			/*    user-defined time pick */
	float	t2;			/*    user-defined time pick */
	float	t3;			/*    user-defined time pick */
	float	t4;			/*    user-defined time pick */
	float	t5;			/*    user-defined time pick */
	float	t6;			/*    user-defined time pick */
	float	t7;			/*    user-defined time pick */
	float	t8;			/*    user-defined time pick */
	float	t9;			/*    user-defined time pick */
	float	f;			/*    event end, sec > nz    */
	float	resp0;			/*    instrument respnse parm*/
	float	resp1;			/*    instrument respnse parm*/
	float	resp2;			/*    instrument respnse parm*/
	float	resp3;			/*    instrument respnse parm*/
	float	resp4;			/*    instrument respnse parm*/
	float	resp5;			/*    instrument respnse parm*/
	float	resp6;			/*    instrument respnse parm*/
	float	resp7;			/*    instrument respnse parm*/
	float	resp8;			/*    instrument respnse parm*/
	float	resp9;			/*    instrument respnse parm*/
	float	stla;			/*  T station latititude     */
	float	stlo;			/*  T station longitude      */
	float	stel;			/*  T station elevation, m   */
	float	stdp;			/*  T station depth, m      */
	float	evla;			/*    event latitude         */
	float	evlo;			/*    event longitude        */
	float	evel;			/*    event elevation        */
	float	evdp;			/*    event depth            */
	float	mag;			/*    reserved for future use*/
	float	user0;			/*    available to user      */
	float	user1;			/*    available to user      */
	float	user2;			/*    available to user      */
	float	user3;			/*    available to user      */
	float	user4;			/*    available to user      */
	float	user5;			/*    available to user      */
	float	user6;			/*    available to user      */
	float	user7;			/*    available to user      */
	float	user8;			/*    available to user      */
	float	user9;			/*    available to user      */
	float	dist;			/*    stn-event distance, km */
	float	az;			/*    event-stn azimuth      */
	float	baz;			/*    stn-event azimuth      */
	float	gcarc;			/*    stn-event dist, degrees*/
	float	sb;			/*    internal use           */
	float	sdelta;			/*    internal use           */
	float	depmen;			/*    mean value, amplitude  */
	float	cmpaz;			/*  T component azimuth     */
	float	cmpinc;			/*  T component inclination */
	float	xminimum;		/*    reserved for future use*/
	float	xmaximum;		/*    reserved for future use*/
	float	yminimum;		/*    reserved for future use*/
	float	ymaximum;		/*    reserved for future use*/
	float	unused6;		/*    reserved for future use*/
	float	unused7;		/*    reserved for future use*/
	float	unused8;		/*    reserved for future use*/
	float	unused9;		/*    reserved for future use*/
	float	unused10;		/*    reserved for future use*/
	float	unused11;		/*    reserved for future use*/
	float	unused12;		/*    reserved for future use*/
	int32 nzyear;			/*  F zero time of file, yr  */
	int32 nzjday;			/*  F zero time of file, day */
	int32 nzhour;			/*  F zero time of file, hr  */
	int32 nzmin;			/*  F zero time of file, min */
	int32 nzsec;			/*  F zero time of file, sec */
	int32 nzmsec;			/*  F zero time of file, millisec*/
	int32 nvhdr;			/*    internal use (version) */
	int32 norid;			/*    origin ID              */
	int32 nevid;			/*    event ID               */
	int32 npts;			/* RF number of samples      */
	int32 nsnpts;			/*    internal use           */
	int32 nwfid;			/*    waveform ID            */
	int32 nxsize;			/*    reserved for future use*/
	int32 nysize;			/*    reserved for future use*/
	int32 unused15;		/*    reserved for future use*/
	int32 iftype;			/* RA type of file          */
	int32 idep;			/*    type of amplitude      */
	int32 iztype;			/*    zero time equivalence  */
	int32 unused16;		/*    reserved for future use*/
	int32 iinst;			/*    recording instrument   */
	int32 istreg;			/*    stn geographic region  */
	int32 ievreg;			/*    event geographic region*/
	int32 ievtyp;			/*    event type             */
	int32 iqual;			/*    quality of data        */
	int32 isynth;			/*    synthetic data flag    */
	int32 imagtyp;		/*    reserved for future use*/
	int32 imagsrc;		/*    reserved for future use*/
	int32 unused19;		/*    reserved for future use*/
	int32 unused20;		/*    reserved for future use*/
	int32 unused21;		/*    reserved for future use*/
	int32 unused22;		/*    reserved for future use*/
	int32 unused23;		/*    reserved for future use*/
	int32 unused24;		/*    reserved for future use*/
	int32 unused25;		/*    reserved for future use*/
	int32 unused26;		/*    reserved for future use*/
	int32 leven;			/* RA data-evenly-spaced flag*/
	int32 lpspol;			/*    station polarity flag  */
	int32 lovrok;			/*    overwrite permission   */
	int32 lcalda;			/*    calc distance, azimuth */
	int32 unused27;		/*    reserved for future use*/
	char	kstnm[8];		/*  F station name           */
	char	kevnm[16];		/*    event name             */
	char	khole[8];		/*    man-made event name    */
	char	ko[8];			/*    event origin time id   */
	char	ka[8];			/*    1st arrival time ident */
	char	kt0[8];			/*    time pick 0 ident      */
	char	kt1[8];			/*    time pick 1 ident      */
	char	kt2[8];			/*    time pick 2 ident      */
	char	kt3[8];			/*    time pick 3 ident      */
	char	kt4[8];			/*    time pick 4 ident      */
	char	kt5[8];			/*    time pick 5 ident      */
	char	kt6[8];			/*    time pick 6 ident      */
	char	kt7[8];			/*    time pick 7 ident      */
	char	kt8[8];			/*    time pick 8 ident      */
	char	kt9[8];			/*    time pick 9 ident      */
	char	kf[8];			/*    end of event ident     */
	char	kuser0[8];		/*    available to user      */
	char	kuser1[8];		/*    available to user      */
	char	kuser2[8];		/*    available to user      */
	char	kcmpnm[8];		/*  F component name         */
	char	knetwk[8];		/*    network name           */
	char	kdatrd[8];		/*    date data read         */
	char	kinst[8];		/*    instrument name        */
};

typedef SACHeader SACHeader_t;

typedef struct
{
    float ud;
    float ew;
    float sn;
} sac_point_t;

class SacFormat
{
public:
    SacFormat();
    ~SacFormat();

    bool open_file(QString filename);
    void close_file();

    bool write_header(QDateTime &dateTime, int sample_freq);
    bool read_header(SACHeader_t *header);

    bool add_point(sac_point_t *point);
    bool read_point(sac_point_t *point);

private:
    int npts;
    QFile *sac_file;
};

#endif // SACFORMAT_H
