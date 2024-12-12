// Copyright 2024 pogyomo
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "tinyc/lex.h"

static const struct {
    const char *value;
    const enum tinyc_token_keyword_kind kind;
} keywords[] = {
    {"auto",       TINYC_TOKEN_AUTO      },
    {"break",      TINYC_TOKEN_BREAK     },
    {"case",       TINYC_TOKEN_CASE      },
    {"char",       TINYC_TOKEN_CHAR      },
    {"const",      TINYC_TOKEN_CONST     },
    {"continue",   TINYC_TOKEN_CONTINUE  },
    {"default",    TINYC_TOKEN_DEFAULT   },
    {"do",         TINYC_TOKEN_DO        },
    {"double",     TINYC_TOKEN_DOUBLE    },
    {"else",       TINYC_TOKEN_ELSE      },
    {"enum",       TINYC_TOKEN_ENUM      },
    {"extern",     TINYC_TOKEN_EXTERN    },
    {"float",      TINYC_TOKEN_FLOAT     },
    {"for",        TINYC_TOKEN_FOR       },
    {"goto",       TINYC_TOKEN_GOTO      },
    {"if",         TINYC_TOKEN_IF        },
    {"inline",     TINYC_TOKEN_INLINE    },
    {"int",        TINYC_TOKEN_INT       },
    {"long",       TINYC_TOKEN_LONG      },
    {"register",   TINYC_TOKEN_REGISTER  },
    {"restrict",   TINYC_TOKEN_RESTRICT  },
    {"return",     TINYC_TOKEN_RETURN    },
    {"short",      TINYC_TOKEN_SHORT     },
    {"signed",     TINYC_TOKEN_SIGNED    },
    {"sizeof",     TINYC_TOKEN_SIZEOF    },
    {"static",     TINYC_TOKEN_STATIC    },
    {"struct",     TINYC_TOKEN_STRUCT    },
    {"switch",     TINYC_TOKEN_SWITCH    },
    {"typedef",    TINYC_TOKEN_TYPEDEF   },
    {"union",      TINYC_TOKEN_UNION     },
    {"unsigned",   TINYC_TOKEN_UNSIGNED  },
    {"void",       TINYC_TOKEN_VOID      },
    {"volatile",   TINYC_TOKEN_VOLATILE  },
    {"while",      TINYC_TOKEN_WHILE     },
    {"_Bool",      TINYC_TOKEN__BOOL     },
    {"_Complex",   TINYC_TOKEN__COMPLEX  },
    {"_Imaginary", TINYC_TOKEN__IMAGINARY},
};

static const struct {
    const char *value;
    const enum tinyc_token_punct_kind kind;
} puncts[] = {
    {"[",    TINYC_TOKEN_LSQUARE  },
    {"]",    TINYC_TOKEN_RSQUARE  },
    {"(",    TINYC_TOKEN_LPAREN   },
    {")",    TINYC_TOKEN_RPAREN   },
    {"{",    TINYC_TOKEN_LCURLY   },
    {"}",    TINYC_TOKEN_RCURLY   },
    {"~",    TINYC_TOKEN_TILDE    },
    {"!=",   TINYC_TOKEN_NE       },
    {"!",    TINYC_TOKEN_EXC      },
    {"==",   TINYC_TOKEN_EQ       },
    {"=",    TINYC_TOKEN_ASSIGN   },
    {"&&",   TINYC_TOKEN_AAMP     },
    {"?",    TINYC_TOKEN_QUESTION },
    {":>",   TINYC_TOKEN_RSQUARE  },
    {":",    TINYC_TOKEN_COLON    },
    {";",    TINYC_TOKEN_SEMICOLON},
    {"...",  TINYC_TOKEN_DDDOT    },
    {".",    TINYC_TOKEN_DOT      },
    {"*=",   TINYC_TOKEN_STAR_A   },
    {"*",    TINYC_TOKEN_STAR     },
    {"/=",   TINYC_TOKEN_SLASH_A  },
    {"/",    TINYC_TOKEN_SLASH    },
    {"%=",   TINYC_TOKEN_PERCENT_A},
    {"%>",   TINYC_TOKEN_RCURLY   },
    {"%:%:", TINYC_TOKEN_SSHARP   },
    {"%:",   TINYC_TOKEN_SHARP    },
    {"%",    TINYC_TOKEN_PERCENT  },
    {"+=",   TINYC_TOKEN_PLUS_A   },
    {"++",   TINYC_TOKEN_PPLUS    },
    {"+",    TINYC_TOKEN_PLUS     },
    {"-=",   TINYC_TOKEN_MINUS_A  },
    {"--",   TINYC_TOKEN_MMINUS   },
    {"->",   TINYC_TOKEN_ARROW    },
    {"-",    TINYC_TOKEN_MINUS    },
    {"<<=",  TINYC_TOKEN_LSHIFT_A },
    {"<<",   TINYC_TOKEN_LSHIFT   },
    {"<:",   TINYC_TOKEN_LSQUARE  },
    {"<%",   TINYC_TOKEN_LCURLY   },
    {"<=",   TINYC_TOKEN_LE       },
    {"<",    TINYC_TOKEN_LT       },
    {">>=",  TINYC_TOKEN_RSHIFT_A },
    {">>",   TINYC_TOKEN_RSHIFT   },
    {">=",   TINYC_TOKEN_GE       },
    {">",    TINYC_TOKEN_GT       },
    {"&=",   TINYC_TOKEN_AMP_A    },
    {"&",    TINYC_TOKEN_AMP      },
    {"^=",   TINYC_TOKEN_HAT_A    },
    {"^",    TINYC_TOKEN_HAT      },
    {"|=",   TINYC_TOKEN_VERT_A   },
    {"||",   TINYC_TOKEN_VVERT    },
    {"|",    TINYC_TOKEN_VERT     },
    {",",    TINYC_TOKEN_COMMA    },
    {"##",   TINYC_TOKEN_SSHARP   },
    {"#",    TINYC_TOKEN_SHARP    },
};

struct tinyc_token_header *tinyc_lex_once(struct tinyc_lex *lex) {}
