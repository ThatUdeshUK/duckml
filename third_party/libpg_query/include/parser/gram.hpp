/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_BASE_YY_THIRD_PARTY_LIBPG_QUERY_GRAMMAR_GRAMMAR_OUT_HPP_INCLUDED
# define YY_BASE_YY_THIRD_PARTY_LIBPG_QUERY_GRAMMAR_GRAMMAR_OUT_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int base_yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    IDENT = 258,                   /* IDENT  */
    FCONST = 259,                  /* FCONST  */
    SCONST = 260,                  /* SCONST  */
    BCONST = 261,                  /* BCONST  */
    XCONST = 262,                  /* XCONST  */
    Op = 263,                      /* Op  */
    ICONST = 264,                  /* ICONST  */
    PARAM = 265,                   /* PARAM  */
    TYPECAST = 266,                /* TYPECAST  */
    DOT_DOT = 267,                 /* DOT_DOT  */
    COLON_EQUALS = 268,            /* COLON_EQUALS  */
    EQUALS_GREATER = 269,          /* EQUALS_GREATER  */
    INTEGER_DIVISION = 270,        /* INTEGER_DIVISION  */
    POWER_OF = 271,                /* POWER_OF  */
    SINGLE_ARROW = 272,            /* SINGLE_ARROW  */
    DOUBLE_ARROW = 273,            /* DOUBLE_ARROW  */
    SINGLE_COLON = 274,            /* SINGLE_COLON  */
    LESS_EQUALS = 275,             /* LESS_EQUALS  */
    GREATER_EQUALS = 276,          /* GREATER_EQUALS  */
    NOT_EQUALS = 277,              /* NOT_EQUALS  */
    ABORT_P = 278,                 /* ABORT_P  */
    ABSOLUTE_P = 279,              /* ABSOLUTE_P  */
    ACCESS = 280,                  /* ACCESS  */
    ACTION = 281,                  /* ACTION  */
    ADD_P = 282,                   /* ADD_P  */
    ADMIN = 283,                   /* ADMIN  */
    AFTER = 284,                   /* AFTER  */
    AGG = 285,                     /* AGG  */
    AGGREGATE = 286,               /* AGGREGATE  */
    ALL = 287,                     /* ALL  */
    ALSO = 288,                    /* ALSO  */
    ALTER = 289,                   /* ALTER  */
    ALWAYS = 290,                  /* ALWAYS  */
    ANALYSE = 291,                 /* ANALYSE  */
    ANALYZE = 292,                 /* ANALYZE  */
    AND = 293,                     /* AND  */
    ANTI = 294,                    /* ANTI  */
    ANY = 295,                     /* ANY  */
    API = 296,                     /* API  */
    ARRAY = 297,                   /* ARRAY  */
    AS = 298,                      /* AS  */
    ASC_P = 299,                   /* ASC_P  */
    ASOF = 300,                    /* ASOF  */
    ASSERTION = 301,               /* ASSERTION  */
    ASSIGNMENT = 302,              /* ASSIGNMENT  */
    ASYMMETRIC = 303,              /* ASYMMETRIC  */
    AT = 304,                      /* AT  */
    ATTACH = 305,                  /* ATTACH  */
    ATTRIBUTE = 306,               /* ATTRIBUTE  */
    AUTHORIZATION = 307,           /* AUTHORIZATION  */
    BACKWARD = 308,                /* BACKWARD  */
    BEFORE = 309,                  /* BEFORE  */
    BEGIN_P = 310,                 /* BEGIN_P  */
    BETWEEN = 311,                 /* BETWEEN  */
    BIGINT = 312,                  /* BIGINT  */
    BINARY = 313,                  /* BINARY  */
    BIT = 314,                     /* BIT  */
    BOOLEAN_P = 315,               /* BOOLEAN_P  */
    BOTH = 316,                    /* BOTH  */
    BY = 317,                      /* BY  */
    CACHE = 318,                   /* CACHE  */
    CALL_P = 319,                  /* CALL_P  */
    CALLED = 320,                  /* CALLED  */
    CASCADE = 321,                 /* CASCADE  */
    CASCADED = 322,                /* CASCADED  */
    CASE = 323,                    /* CASE  */
    CAST = 324,                    /* CAST  */
    CATALOG_P = 325,               /* CATALOG_P  */
    CENTURIES_P = 326,             /* CENTURIES_P  */
    CENTURY_P = 327,               /* CENTURY_P  */
    CHAIN = 328,                   /* CHAIN  */
    CHAR_P = 329,                  /* CHAR_P  */
    CHARACTER = 330,               /* CHARACTER  */
    CHARACTERISTICS = 331,         /* CHARACTERISTICS  */
    CHECK_P = 332,                 /* CHECK_P  */
    CHECKPOINT = 333,              /* CHECKPOINT  */
    CLASS = 334,                   /* CLASS  */
    CLOSE = 335,                   /* CLOSE  */
    CLUSTER = 336,                 /* CLUSTER  */
    COALESCE = 337,                /* COALESCE  */
    COLLATE = 338,                 /* COLLATE  */
    COLLATION = 339,               /* COLLATION  */
    COLUMN = 340,                  /* COLUMN  */
    COLUMNS = 341,                 /* COLUMNS  */
    COMMENT = 342,                 /* COMMENT  */
    COMMENTS = 343,                /* COMMENTS  */
    COMMIT = 344,                  /* COMMIT  */
    COMMITTED = 345,               /* COMMITTED  */
    COMPRESSION = 346,             /* COMPRESSION  */
    CONCURRENTLY = 347,            /* CONCURRENTLY  */
    CONFIGURATION = 348,           /* CONFIGURATION  */
    CONFLICT = 349,                /* CONFLICT  */
    CONNECTION = 350,              /* CONNECTION  */
    CONSTRAINT = 351,              /* CONSTRAINT  */
    CONSTRAINTS = 352,             /* CONSTRAINTS  */
    CONTENT_P = 353,               /* CONTENT_P  */
    CONTINUE_P = 354,              /* CONTINUE_P  */
    CONVERSION_P = 355,            /* CONVERSION_P  */
    COPY = 356,                    /* COPY  */
    COST = 357,                    /* COST  */
    CREATE_P = 358,                /* CREATE_P  */
    CROSS = 359,                   /* CROSS  */
    CSV = 360,                     /* CSV  */
    CUBE = 361,                    /* CUBE  */
    CURRENT_P = 362,               /* CURRENT_P  */
    CURSOR = 363,                  /* CURSOR  */
    CYCLE = 364,                   /* CYCLE  */
    DATA_P = 365,                  /* DATA_P  */
    DATABASE = 366,                /* DATABASE  */
    DAY_P = 367,                   /* DAY_P  */
    DAYS_P = 368,                  /* DAYS_P  */
    DEALLOCATE = 369,              /* DEALLOCATE  */
    DEC = 370,                     /* DEC  */
    DECADE_P = 371,                /* DECADE_P  */
    DECADES_P = 372,               /* DECADES_P  */
    DECIMAL_P = 373,               /* DECIMAL_P  */
    DECLARE = 374,                 /* DECLARE  */
    DEFAULT = 375,                 /* DEFAULT  */
    DEFAULTS = 376,                /* DEFAULTS  */
    DEFERRABLE = 377,              /* DEFERRABLE  */
    DEFERRED = 378,                /* DEFERRED  */
    DEFINER = 379,                 /* DEFINER  */
    DELETE_P = 380,                /* DELETE_P  */
    DELIMITER = 381,               /* DELIMITER  */
    DELIMITERS = 382,              /* DELIMITERS  */
    DEPENDS = 383,                 /* DEPENDS  */
    DESC_P = 384,                  /* DESC_P  */
    DESCRIBE = 385,                /* DESCRIBE  */
    DETACH = 386,                  /* DETACH  */
    DICTIONARY = 387,              /* DICTIONARY  */
    DISABLE_P = 388,               /* DISABLE_P  */
    DISCARD = 389,                 /* DISCARD  */
    DISTINCT = 390,                /* DISTINCT  */
    DO = 391,                      /* DO  */
    DOCUMENT_P = 392,              /* DOCUMENT_P  */
    DOMAIN_P = 393,                /* DOMAIN_P  */
    DOUBLE_P = 394,                /* DOUBLE_P  */
    DROP = 395,                    /* DROP  */
    EACH = 396,                    /* EACH  */
    EDGES = 397,                   /* EDGES  */
    ELSE = 398,                    /* ELSE  */
    ENABLE_P = 399,                /* ENABLE_P  */
    ENCODING = 400,                /* ENCODING  */
    ENCRYPTED = 401,               /* ENCRYPTED  */
    END_P = 402,                   /* END_P  */
    ENUM_P = 403,                  /* ENUM_P  */
    ESCAPE = 404,                  /* ESCAPE  */
    EVENT = 405,                   /* EVENT  */
    EXCEPT = 406,                  /* EXCEPT  */
    EXCLUDE = 407,                 /* EXCLUDE  */
    EXCLUDING = 408,               /* EXCLUDING  */
    EXCLUSIVE = 409,               /* EXCLUSIVE  */
    EXECUTE = 410,                 /* EXECUTE  */
    EXISTS = 411,                  /* EXISTS  */
    EXPLAIN = 412,                 /* EXPLAIN  */
    EXPORT_P = 413,                /* EXPORT_P  */
    EXPORT_STATE = 414,            /* EXPORT_STATE  */
    EXTENSION = 415,               /* EXTENSION  */
    EXTENSIONS = 416,              /* EXTENSIONS  */
    EXTERNAL = 417,                /* EXTERNAL  */
    EXTRACT = 418,                 /* EXTRACT  */
    FALSE_P = 419,                 /* FALSE_P  */
    FAMILY = 420,                  /* FAMILY  */
    FEATURES = 421,                /* FEATURES  */
    FETCH = 422,                   /* FETCH  */
    FILTER = 423,                  /* FILTER  */
    FIRST_P = 424,                 /* FIRST_P  */
    FLOAT_P = 425,                 /* FLOAT_P  */
    FOLLOWING = 426,               /* FOLLOWING  */
    FOR = 427,                     /* FOR  */
    FORCE = 428,                   /* FORCE  */
    FOREIGN = 429,                 /* FOREIGN  */
    FORWARD = 430,                 /* FORWARD  */
    FREEZE = 431,                  /* FREEZE  */
    FROM = 432,                    /* FROM  */
    FULL = 433,                    /* FULL  */
    FUNCTION = 434,                /* FUNCTION  */
    FUNCTIONS = 435,               /* FUNCTIONS  */
    GENERATED = 436,               /* GENERATED  */
    GLOB = 437,                    /* GLOB  */
    GLOBAL = 438,                  /* GLOBAL  */
    GNN = 439,                     /* GNN  */
    GRANT = 440,                   /* GRANT  */
    GRANTED = 441,                 /* GRANTED  */
    GROUP_P = 442,                 /* GROUP_P  */
    GROUPING = 443,                /* GROUPING  */
    GROUPING_ID = 444,             /* GROUPING_ID  */
    GROUPS = 445,                  /* GROUPS  */
    HANDLER = 446,                 /* HANDLER  */
    HAVING = 447,                  /* HAVING  */
    HEADER_P = 448,                /* HEADER_P  */
    HOLD = 449,                    /* HOLD  */
    HOUR_P = 450,                  /* HOUR_P  */
    HOURS_P = 451,                 /* HOURS_P  */
    IDENTITY_P = 452,              /* IDENTITY_P  */
    IF_P = 453,                    /* IF_P  */
    IGNORE_P = 454,                /* IGNORE_P  */
    ILIKE = 455,                   /* ILIKE  */
    IMMEDIATE = 456,               /* IMMEDIATE  */
    IMMUTABLE = 457,               /* IMMUTABLE  */
    IMPLICIT_P = 458,              /* IMPLICIT_P  */
    IMPORT_P = 459,                /* IMPORT_P  */
    IN_P = 460,                    /* IN_P  */
    INCLUDE_P = 461,               /* INCLUDE_P  */
    INCLUDING = 462,               /* INCLUDING  */
    INCREMENT = 463,               /* INCREMENT  */
    INDEX = 464,                   /* INDEX  */
    INDEXES = 465,                 /* INDEXES  */
    INHERIT = 466,                 /* INHERIT  */
    INHERITS = 467,                /* INHERITS  */
    INITIALLY = 468,               /* INITIALLY  */
    INLINE_P = 469,                /* INLINE_P  */
    INNER_P = 470,                 /* INNER_P  */
    INOUT = 471,                   /* INOUT  */
    INPUT_P = 472,                 /* INPUT_P  */
    INSENSITIVE = 473,             /* INSENSITIVE  */
    INSERT = 474,                  /* INSERT  */
    INSTALL = 475,                 /* INSTALL  */
    INSTEAD = 476,                 /* INSTEAD  */
    INT_P = 477,                   /* INT_P  */
    INTEGER = 478,                 /* INTEGER  */
    INTERSECT = 479,               /* INTERSECT  */
    INTERVAL = 480,                /* INTERVAL  */
    INTO = 481,                    /* INTO  */
    INVOKER = 482,                 /* INVOKER  */
    IS = 483,                      /* IS  */
    ISNULL = 484,                  /* ISNULL  */
    ISOLATION = 485,               /* ISOLATION  */
    JOIN = 486,                    /* JOIN  */
    JSON = 487,                    /* JSON  */
    KEY = 488,                     /* KEY  */
    LABEL = 489,                   /* LABEL  */
    LAMBDA = 490,                  /* LAMBDA  */
    LANGUAGE = 491,                /* LANGUAGE  */
    LARGE_P = 492,                 /* LARGE_P  */
    LAST_P = 493,                  /* LAST_P  */
    LATERAL_P = 494,               /* LATERAL_P  */
    LEADING = 495,                 /* LEADING  */
    LEAKPROOF = 496,               /* LEAKPROOF  */
    LEFT = 497,                    /* LEFT  */
    LEVEL = 498,                   /* LEVEL  */
    LIKE = 499,                    /* LIKE  */
    LIMIT = 500,                   /* LIMIT  */
    LISTEN = 501,                  /* LISTEN  */
    LLM = 502,                     /* LLM  */
    LM = 503,                      /* LM  */
    LOAD = 504,                    /* LOAD  */
    LOCAL = 505,                   /* LOCAL  */
    LOCATION = 506,                /* LOCATION  */
    LOCK_P = 507,                  /* LOCK_P  */
    LOCKED = 508,                  /* LOCKED  */
    LOGGED = 509,                  /* LOGGED  */
    MACRO = 510,                   /* MACRO  */
    MAP = 511,                     /* MAP  */
    MAPPING = 512,                 /* MAPPING  */
    MATCH = 513,                   /* MATCH  */
    MATERIALIZED = 514,            /* MATERIALIZED  */
    MAXVALUE = 515,                /* MAXVALUE  */
    METHOD = 516,                  /* METHOD  */
    MICROSECOND_P = 517,           /* MICROSECOND_P  */
    MICROSECONDS_P = 518,          /* MICROSECONDS_P  */
    MILLENNIA_P = 519,             /* MILLENNIA_P  */
    MILLENNIUM_P = 520,            /* MILLENNIUM_P  */
    MILLISECOND_P = 521,           /* MILLISECOND_P  */
    MILLISECONDS_P = 522,          /* MILLISECONDS_P  */
    MINUTE_P = 523,                /* MINUTE_P  */
    MINUTES_P = 524,               /* MINUTES_P  */
    MINVALUE = 525,                /* MINVALUE  */
    MODE = 526,                    /* MODE  */
    MODEL = 527,                   /* MODEL  */
    MONTH_P = 528,                 /* MONTH_P  */
    MONTHS_P = 529,                /* MONTHS_P  */
    MOVE = 530,                    /* MOVE  */
    NAME_P = 531,                  /* NAME_P  */
    NAMES = 532,                   /* NAMES  */
    NATIONAL = 533,                /* NATIONAL  */
    NATURAL = 534,                 /* NATURAL  */
    NCHAR = 535,                   /* NCHAR  */
    NEW = 536,                     /* NEW  */
    NEXT = 537,                    /* NEXT  */
    NO = 538,                      /* NO  */
    NODES = 539,                   /* NODES  */
    NONE = 540,                    /* NONE  */
    NOT = 541,                     /* NOT  */
    NOTHING = 542,                 /* NOTHING  */
    NOTIFY = 543,                  /* NOTIFY  */
    NOTNULL = 544,                 /* NOTNULL  */
    NOWAIT = 545,                  /* NOWAIT  */
    NULL_P = 546,                  /* NULL_P  */
    NULLIF = 547,                  /* NULLIF  */
    NULLS_P = 548,                 /* NULLS_P  */
    NUMERIC = 549,                 /* NUMERIC  */
    OBJECT_P = 550,                /* OBJECT_P  */
    OF = 551,                      /* OF  */
    OFF = 552,                     /* OFF  */
    OFFSET = 553,                  /* OFFSET  */
    OIDS = 554,                    /* OIDS  */
    OLD = 555,                     /* OLD  */
    ON = 556,                      /* ON  */
    ONLY = 557,                    /* ONLY  */
    OPERATOR = 558,                /* OPERATOR  */
    OPTION = 559,                  /* OPTION  */
    OPTIONS = 560,                 /* OPTIONS  */
    OR = 561,                      /* OR  */
    ORDER = 562,                   /* ORDER  */
    ORDINALITY = 563,              /* ORDINALITY  */
    OTHERS = 564,                  /* OTHERS  */
    OUT_P = 565,                   /* OUT_P  */
    OUTER_P = 566,                 /* OUTER_P  */
    OUTPUT = 567,                  /* OUTPUT  */
    OVER = 568,                    /* OVER  */
    OVERLAPS = 569,                /* OVERLAPS  */
    OVERLAY = 570,                 /* OVERLAY  */
    OVERRIDING = 571,              /* OVERRIDING  */
    OWNED = 572,                   /* OWNED  */
    OWNER = 573,                   /* OWNER  */
    PARALLEL = 574,                /* PARALLEL  */
    PARSER = 575,                  /* PARSER  */
    PARTIAL = 576,                 /* PARTIAL  */
    PARTITION = 577,               /* PARTITION  */
    PARTITIONED = 578,             /* PARTITIONED  */
    PASSING = 579,                 /* PASSING  */
    PASSWORD = 580,                /* PASSWORD  */
    PATH = 581,                    /* PATH  */
    PERCENT = 582,                 /* PERCENT  */
    PERSISTENT = 583,              /* PERSISTENT  */
    PIVOT = 584,                   /* PIVOT  */
    PIVOT_LONGER = 585,            /* PIVOT_LONGER  */
    PIVOT_WIDER = 586,             /* PIVOT_WIDER  */
    PLACING = 587,                 /* PLACING  */
    PLANS = 588,                   /* PLANS  */
    POLICY = 589,                  /* POLICY  */
    POSITION = 590,                /* POSITION  */
    POSITIONAL = 591,              /* POSITIONAL  */
    PRAGMA_P = 592,                /* PRAGMA_P  */
    PRECEDING = 593,               /* PRECEDING  */
    PRECISION = 594,               /* PRECISION  */
    PREDICT = 595,                 /* PREDICT  */
    PREPARE = 596,                 /* PREPARE  */
    PREPARED = 597,                /* PREPARED  */
    PRESERVE = 598,                /* PRESERVE  */
    PRIMARY = 599,                 /* PRIMARY  */
    PRIOR = 600,                   /* PRIOR  */
    PRIVILEGES = 601,              /* PRIVILEGES  */
    PROCEDURAL = 602,              /* PROCEDURAL  */
    PROCEDURE = 603,               /* PROCEDURE  */
    PROGRAM = 604,                 /* PROGRAM  */
    PROMPT = 605,                  /* PROMPT  */
    PUBLICATION = 606,             /* PUBLICATION  */
    QUALIFY = 607,                 /* QUALIFY  */
    QUARTER_P = 608,               /* QUARTER_P  */
    QUARTERS_P = 609,              /* QUARTERS_P  */
    QUOTE = 610,                   /* QUOTE  */
    RANGE = 611,                   /* RANGE  */
    READ_P = 612,                  /* READ_P  */
    REAL = 613,                    /* REAL  */
    REASSIGN = 614,                /* REASSIGN  */
    RECHECK = 615,                 /* RECHECK  */
    RECURSIVE = 616,               /* RECURSIVE  */
    REF = 617,                     /* REF  */
    REFERENCES = 618,              /* REFERENCES  */
    REFERENCING = 619,             /* REFERENCING  */
    REFRESH = 620,                 /* REFRESH  */
    REINDEX = 621,                 /* REINDEX  */
    RELATIVE_P = 622,              /* RELATIVE_P  */
    RELEASE = 623,                 /* RELEASE  */
    RENAME = 624,                  /* RENAME  */
    REPEATABLE = 625,              /* REPEATABLE  */
    REPLACE = 626,                 /* REPLACE  */
    REPLICA = 627,                 /* REPLICA  */
    RESET = 628,                   /* RESET  */
    RESPECT_P = 629,               /* RESPECT_P  */
    RESTART = 630,                 /* RESTART  */
    RESTRICT = 631,                /* RESTRICT  */
    RETURNING = 632,               /* RETURNING  */
    RETURNS = 633,                 /* RETURNS  */
    REVOKE = 634,                  /* REVOKE  */
    RIGHT = 635,                   /* RIGHT  */
    ROLE = 636,                    /* ROLE  */
    ROLLBACK = 637,                /* ROLLBACK  */
    ROLLUP = 638,                  /* ROLLUP  */
    ROW = 639,                     /* ROW  */
    ROWS = 640,                    /* ROWS  */
    RULE = 641,                    /* RULE  */
    SAMPLE = 642,                  /* SAMPLE  */
    SAVEPOINT = 643,               /* SAVEPOINT  */
    SCHEMA = 644,                  /* SCHEMA  */
    SCHEMAS = 645,                 /* SCHEMAS  */
    SCOPE = 646,                   /* SCOPE  */
    SCROLL = 647,                  /* SCROLL  */
    SEARCH = 648,                  /* SEARCH  */
    SECOND_P = 649,                /* SECOND_P  */
    SECONDS_P = 650,               /* SECONDS_P  */
    SECRET = 651,                  /* SECRET  */
    SECURITY = 652,                /* SECURITY  */
    SELECT = 653,                  /* SELECT  */
    SEMI = 654,                    /* SEMI  */
    SEQUENCE = 655,                /* SEQUENCE  */
    SEQUENCES = 656,               /* SEQUENCES  */
    SERIALIZABLE = 657,            /* SERIALIZABLE  */
    SERVER = 658,                  /* SERVER  */
    SESSION = 659,                 /* SESSION  */
    SET = 660,                     /* SET  */
    SETOF = 661,                   /* SETOF  */
    SETS = 662,                    /* SETS  */
    SHARE = 663,                   /* SHARE  */
    SHOW = 664,                    /* SHOW  */
    SIMILAR = 665,                 /* SIMILAR  */
    SIMPLE = 666,                  /* SIMPLE  */
    SKIP = 667,                    /* SKIP  */
    SMALLINT = 668,                /* SMALLINT  */
    SNAPSHOT = 669,                /* SNAPSHOT  */
    SOME = 670,                    /* SOME  */
    SORTED = 671,                  /* SORTED  */
    SQL_P = 672,                   /* SQL_P  */
    STABLE = 673,                  /* STABLE  */
    STANDALONE_P = 674,            /* STANDALONE_P  */
    START = 675,                   /* START  */
    STATEMENT = 676,               /* STATEMENT  */
    STATISTICS = 677,              /* STATISTICS  */
    STDIN = 678,                   /* STDIN  */
    STDOUT = 679,                  /* STDOUT  */
    STORAGE = 680,                 /* STORAGE  */
    STORED = 681,                  /* STORED  */
    STRICT_P = 682,                /* STRICT_P  */
    STRIP_P = 683,                 /* STRIP_P  */
    STRUCT = 684,                  /* STRUCT  */
    SUBSCRIPTION = 685,            /* SUBSCRIPTION  */
    SUBSTRING = 686,               /* SUBSTRING  */
    SUMMARIZE = 687,               /* SUMMARIZE  */
    SYMMETRIC = 688,               /* SYMMETRIC  */
    SYSID = 689,                   /* SYSID  */
    SYSTEM_P = 690,                /* SYSTEM_P  */
    TABLE = 691,                   /* TABLE  */
    TABLES = 692,                  /* TABLES  */
    TABLESAMPLE = 693,             /* TABLESAMPLE  */
    TABLESPACE = 694,              /* TABLESPACE  */
    TABULAR = 695,                 /* TABULAR  */
    TAKES = 696,                   /* TAKES  */
    TEMP = 697,                    /* TEMP  */
    TEMPLATE = 698,                /* TEMPLATE  */
    TEMPORARY = 699,               /* TEMPORARY  */
    TEXT_P = 700,                  /* TEXT_P  */
    THEN = 701,                    /* THEN  */
    TIES = 702,                    /* TIES  */
    TIME = 703,                    /* TIME  */
    TIMESTAMP = 704,               /* TIMESTAMP  */
    TO = 705,                      /* TO  */
    TRAILING = 706,                /* TRAILING  */
    TRANSACTION = 707,             /* TRANSACTION  */
    TRANSFORM = 708,               /* TRANSFORM  */
    TREAT = 709,                   /* TREAT  */
    TRIGGER = 710,                 /* TRIGGER  */
    TRIM = 711,                    /* TRIM  */
    TRUE_P = 712,                  /* TRUE_P  */
    TRUNCATE = 713,                /* TRUNCATE  */
    TRUSTED = 714,                 /* TRUSTED  */
    TRY_CAST = 715,                /* TRY_CAST  */
    TYPE_P = 716,                  /* TYPE_P  */
    TYPES_P = 717,                 /* TYPES_P  */
    UNBOUNDED = 718,               /* UNBOUNDED  */
    UNCOMMITTED = 719,             /* UNCOMMITTED  */
    UNENCRYPTED = 720,             /* UNENCRYPTED  */
    UNION = 721,                   /* UNION  */
    UNIQUE = 722,                  /* UNIQUE  */
    UNKNOWN = 723,                 /* UNKNOWN  */
    UNLISTEN = 724,                /* UNLISTEN  */
    UNLOGGED = 725,                /* UNLOGGED  */
    UNPACK = 726,                  /* UNPACK  */
    UNPIVOT = 727,                 /* UNPIVOT  */
    UNTIL = 728,                   /* UNTIL  */
    UPDATE = 729,                  /* UPDATE  */
    USE_P = 730,                   /* USE_P  */
    USER = 731,                    /* USER  */
    USING = 732,                   /* USING  */
    VACUUM = 733,                  /* VACUUM  */
    VALID = 734,                   /* VALID  */
    VALIDATE = 735,                /* VALIDATE  */
    VALIDATOR = 736,               /* VALIDATOR  */
    VALUE_P = 737,                 /* VALUE_P  */
    VALUES = 738,                  /* VALUES  */
    VARCHAR = 739,                 /* VARCHAR  */
    VARIABLE_P = 740,              /* VARIABLE_P  */
    VARIADIC = 741,                /* VARIADIC  */
    VARYING = 742,                 /* VARYING  */
    VERBOSE = 743,                 /* VERBOSE  */
    VERSION_P = 744,               /* VERSION_P  */
    VIEW = 745,                    /* VIEW  */
    VIEWS = 746,                   /* VIEWS  */
    VIRTUAL = 747,                 /* VIRTUAL  */
    VOLATILE = 748,                /* VOLATILE  */
    WEEK_P = 749,                  /* WEEK_P  */
    WEEKS_P = 750,                 /* WEEKS_P  */
    WHEN = 751,                    /* WHEN  */
    WHERE = 752,                   /* WHERE  */
    WHITESPACE_P = 753,            /* WHITESPACE_P  */
    WINDOW = 754,                  /* WINDOW  */
    WITH = 755,                    /* WITH  */
    WITHIN = 756,                  /* WITHIN  */
    WITHOUT = 757,                 /* WITHOUT  */
    WORK = 758,                    /* WORK  */
    WRAPPER = 759,                 /* WRAPPER  */
    WRITE_P = 760,                 /* WRITE_P  */
    XML_P = 761,                   /* XML_P  */
    XMLATTRIBUTES = 762,           /* XMLATTRIBUTES  */
    XMLCONCAT = 763,               /* XMLCONCAT  */
    XMLELEMENT = 764,              /* XMLELEMENT  */
    XMLEXISTS = 765,               /* XMLEXISTS  */
    XMLFOREST = 766,               /* XMLFOREST  */
    XMLNAMESPACES = 767,           /* XMLNAMESPACES  */
    XMLPARSE = 768,                /* XMLPARSE  */
    XMLPI = 769,                   /* XMLPI  */
    XMLROOT = 770,                 /* XMLROOT  */
    XMLSERIALIZE = 771,            /* XMLSERIALIZE  */
    XMLTABLE = 772,                /* XMLTABLE  */
    YEAR_P = 773,                  /* YEAR_P  */
    YEARS_P = 774,                 /* YEARS_P  */
    YES_P = 775,                   /* YES_P  */
    ZONE = 776,                    /* ZONE  */
    NOT_LA = 777,                  /* NOT_LA  */
    NULLS_LA = 778,                /* NULLS_LA  */
    WITH_LA = 779,                 /* WITH_LA  */
    POSTFIXOP = 780,               /* POSTFIXOP  */
    UMINUS = 781                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 14 "third_party/libpg_query/grammar/grammar.y"

	core_YYSTYPE		core_yystype;
	/* these fields must match core_YYSTYPE: */
	int					ival;
	char				*str;
	const char			*keyword;
	const char          *conststr;

	char				chr;
	bool				boolean;
	PGJoinType			jtype;
	PGDropBehavior		dbehavior;
	PGOnCommitAction		oncommit;
	PGOnCreateConflict		oncreateconflict;
	PGList				*list;
	PGNode				*node;
	PGValue				*value;
	PGObjectType			objtype;
	PGTypeName			*typnam;
	PGObjectWithArgs		*objwithargs;
	PGDefElem				*defelt;
	PGSortBy				*sortby;
	PGWindowDef			*windef;
	PGJoinExpr			*jexpr;
	PGIndexElem			*ielem;
	PGAlias				*alias;
	PGRangeVar			*range;
	PGIntoClause			*into;
	PGCTEMaterialize			ctematerialize;
	PGWithClause			*with;
	PGInferClause			*infer;
	PGOnConflictClause	*onconflict;
	PGOnConflictActionAlias onconflictshorthand;
	PGAIndices			*aind;
	PGResTarget			*target;
	PGInsertStmt			*istmt;
	PGVariableSetStmt		*vsetstmt;
	PGOverridingKind       override;
	PGSortByDir            sortorder;
	PGSortByNulls          nullorder;
	PGIgnoreNulls          ignorenulls;
	PGConstrType           constr;
	PGLockClauseStrength lockstrength;
	PGLockWaitPolicy lockwaitpolicy;
	PGSubLinkType subquerytype;
	PGViewCheckOption viewcheckoption;
	PGInsertColumnOrder bynameorposition;
	PGLoadInstallType loadinstalltype;
	PGTransactionStmtType transactiontype;

#line 641 "third_party/libpg_query/grammar/grammar_out.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif




int base_yyparse (core_yyscan_t yyscanner);


#endif /* !YY_BASE_YY_THIRD_PARTY_LIBPG_QUERY_GRAMMAR_GRAMMAR_OUT_HPP_INCLUDED  */
