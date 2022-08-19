/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "JetBrainsMono-Regular:size=11" };
static const char dmenufont[]       = "JetBrainsMono-Regular:size=11";
static const char col_gray1[]       = "#21262d";
static const char col_gray2[]       = "#21262d";
static const char col_gray3[]       = "#a9b1d6";
static const char col_gray4[]       = "#c0caf5";
static const char col_cyan[]        = "#005577";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 0; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", NULL  };
static const char *termcmd[]  = { "st", NULL };
static const char *upvol[]   = { "/usr/bin/pactl", "set-sink-volume", "0", "+5%",     NULL };
static const char *downvol[] = { "/usr/bin/pactl", "set-sink-volume", "0", "-5%",     NULL };
static const char *mutevol[] = { "/usr/bin/pactl", "set-sink-mute",   "0", "toggle",  NULL };
static const char *mic[] = { "/usr/bin/pactl", "set-source-mute ",   "0", "toggle",  NULL };
static const char *light_up[] = {"/usr/bin/brightnessctl", "set", "+5%", NULL};
static const char *light_down[] = {"/usr/bin/brightnessctl", "set", "5%-", NULL};
static const char *btn_prev[] = {"/usr/bin/mpc", "prev", NULL, NULL};
static const char *btn_pause[] = {"/usr/bin/mpc", "pause", NULL, NULL};
// static const char *btn_play[] = {"/usr/bin/mpc", "play", NULL, NULL};
static const char *btn_toggle[] = {"/usr/bin/mpc", "toggle", NULL, NULL};
static const char *btn_next[] = {"/usr/bin/mpc", "next", NULL, NULL};



static Key keys[] = {
	/* modifier                     key        				function        			argument */
	{ MODKEY,                       XK_p,      				spawn,          			{.v = dmenucmd } },
	{ MODKEY,             			XK_Return, 				spawn,          			{.v = termcmd } },
	{ MODKEY,                       XK_b,      				togglebar,      			{0} },
	{ MODKEY,                       XK_j,      				focusstack,     			{.i = +1 } },
	{ MODKEY,                       XK_k,      				focusstack,     			{.i = -1 } },
	{ MODKEY,                       XK_h,          			shiftview,      			{.i = -1 } },
	{ MODKEY,                       XK_l,          			shiftview,      			{.i = +1 } },
	// { MODKEY,                       XK_i,      			incnmaster,     			{.i = +1 } },
	// { MODKEY,                       XK_d,      			ncnmaster,     				{.i = -1 } },
	// { MODKEY,                       XK_h,      			setmfact,       			{.f = -0.05} },
	// { MODKEY,                       XK_l,      			setmfact,       			{.f = +0.05} },
	{ MODKEY,                       XK_space, 				zoom,           			{0} },
	{ MODKEY,                       XK_Tab,    				view,           			{0} },
	{ MODKEY,             			XK_q,      				killclient,     			{0} },
	{ MODKEY|ShiftMask,             XK_q,          			quit,           			{0} },
	{ MODKEY,                       XK_t,      				setlayout,      			{.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      				setlayout,      			{.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      				setlayout,      			{.v = &layouts[2]} },
	{ MODKEY|ShiftMask,             XK_space,  				togglefloating, 			{0} },
	{ MODKEY,                       XK_0,      				view,           			{.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      				tag,            			{.ui = ~0 } },
	{ MODKEY,                       XK_bracketleft,  		focusmon,       			{.i = -1 } },
	{ MODKEY,                       XK_bracketleft, 		focusmon,       			{.i = +1 } },
	{ MODKEY|ShiftMask,             XK_bracketleft,  		tagmon,         			{.i = -1 } },
	{ MODKEY|ShiftMask,             XK_bracketleft, 		tagmon,         			{.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ 0,    						XF86XK_AudioLowerVolume,	 spawn,		    		{.v = downvol } },
	{ 0,							XF86XK_AudioMute,		     spawn,		    		{.v = mutevol } },
	{ 0,							XF86XK_AudioRaiseVolume,	 spawn,		    		{.v = upvol   } },
	{ 0,							XF86XK_AudioMicMute,	     spawn,		    		{.v = mic   } },
    { 0,							XF86XK_MonBrightnessUp,		 spawn,	        		{.v = light_up} },
	{ 0,							XF86XK_MonBrightnessDown,	 spawn,	        		{.v = light_down} },
	{ 0,							XF86XK_AudioPrev,			 spawn,	        		{.v = btn_prev} },
	{ 0,							XF86XK_AudioPause,			 spawn,	        		{.v = btn_pause} },
	{ 0,							XF86XK_AudioPlay,			 spawn,	        		{.v = btn_toggle} },
	{ 0,							XF86XK_AudioNext,			 spawn,	        		{.v = btn_next} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

