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
    EMBED = 399,                   /* EMBED  */
    ENABLE_P = 400,                /* ENABLE_P  */
    ENCODING = 401,                /* ENCODING  */
    ENCRYPTED = 402,               /* ENCRYPTED  */
    END_P = 403,                   /* END_P  */
    ENUM_P = 404,                  /* ENUM_P  */
    ERROR_P = 405,                 /* ERROR_P  */
    ESCAPE = 406,                  /* ESCAPE  */
    EVENT = 407,                   /* EVENT  */
    EXCEPT = 408,                  /* EXCEPT  */
    EXCLUDE = 409,                 /* EXCLUDE  */
    EXCLUDING = 410,               /* EXCLUDING  */
    EXCLUSIVE = 411,               /* EXCLUSIVE  */
    EXECUTE = 412,                 /* EXECUTE  */
    EXISTS = 413,                  /* EXISTS  */
    EXPLAIN = 414,                 /* EXPLAIN  */
    EXPORT_P = 415,                /* EXPORT_P  */
    EXPORT_STATE = 416,            /* EXPORT_STATE  */
    EXTENSION = 417,               /* EXTENSION  */
    EXTENSIONS = 418,              /* EXTENSIONS  */
    EXTERNAL = 419,                /* EXTERNAL  */
    EXTRACT = 420,                 /* EXTRACT  */
    FALSE_P = 421,                 /* FALSE_P  */
    FAMILY = 422,                  /* FAMILY  */
    FEATURES = 423,                /* FEATURES  */
    FETCH = 424,                   /* FETCH  */
    FILTER = 425,                  /* FILTER  */
    FIRST_P = 426,                 /* FIRST_P  */
    FLOAT_P = 427,                 /* FLOAT_P  */
    FOLLOWING = 428,               /* FOLLOWING  */
    FOR = 429,                     /* FOR  */
    FORCE = 430,                   /* FORCE  */
    FOREIGN = 431,                 /* FOREIGN  */
    FORWARD = 432,                 /* FORWARD  */
    FREEZE = 433,                  /* FREEZE  */
    FROM = 434,                    /* FROM  */
    FULL = 435,                    /* FULL  */
    FUNCTION = 436,                /* FUNCTION  */
    FUNCTIONS = 437,               /* FUNCTIONS  */
    GENERATED = 438,               /* GENERATED  */
    GLOB = 439,                    /* GLOB  */
    GLOBAL = 440,                  /* GLOBAL  */
    GNN = 441,                     /* GNN  */
    GRANT = 442,                   /* GRANT  */
    GRANTED = 443,                 /* GRANTED  */
    GROUP_P = 444,                 /* GROUP_P  */
    GROUPING = 445,                /* GROUPING  */
    GROUPING_ID = 446,             /* GROUPING_ID  */
    GROUPS = 447,                  /* GROUPS  */
    HANDLER = 448,                 /* HANDLER  */
    HAVING = 449,                  /* HAVING  */
    HEADER_P = 450,                /* HEADER_P  */
    HOLD = 451,                    /* HOLD  */
    HOUR_P = 452,                  /* HOUR_P  */
    HOURS_P = 453,                 /* HOURS_P  */
    IDENTITY_P = 454,              /* IDENTITY_P  */
    IF_P = 455,                    /* IF_P  */
    IGNORE_P = 456,                /* IGNORE_P  */
    ILIKE = 457,                   /* ILIKE  */
    IMMEDIATE = 458,               /* IMMEDIATE  */
    IMMUTABLE = 459,               /* IMMUTABLE  */
    IMPLICIT_P = 460,              /* IMPLICIT_P  */
    IMPORT_P = 461,                /* IMPORT_P  */
    IN_P = 462,                    /* IN_P  */
    INCLUDE_P = 463,               /* INCLUDE_P  */
    INCLUDING = 464,               /* INCLUDING  */
    INCREMENT = 465,               /* INCREMENT  */
    INDEX = 466,                   /* INDEX  */
    INDEXES = 467,                 /* INDEXES  */
    INHERIT = 468,                 /* INHERIT  */
    INHERITS = 469,                /* INHERITS  */
    INITIALLY = 470,               /* INITIALLY  */
    INLINE_P = 471,                /* INLINE_P  */
    INNER_P = 472,                 /* INNER_P  */
    INOUT = 473,                   /* INOUT  */
    INPUT_P = 474,                 /* INPUT_P  */
    INSENSITIVE = 475,             /* INSENSITIVE  */
    INSERT = 476,                  /* INSERT  */
    INSTALL = 477,                 /* INSTALL  */
    INSTEAD = 478,                 /* INSTEAD  */
    INT_P = 479,                   /* INT_P  */
    INTEGER = 480,                 /* INTEGER  */
    INTERSECT = 481,               /* INTERSECT  */
    INTERVAL = 482,                /* INTERVAL  */
    INTO = 483,                    /* INTO  */
    INVOKER = 484,                 /* INVOKER  */
    IS = 485,                      /* IS  */
    ISNULL = 486,                  /* ISNULL  */
    ISOLATION = 487,               /* ISOLATION  */
    JOIN = 488,                    /* JOIN  */
    JSON = 489,                    /* JSON  */
    KEY = 490,                     /* KEY  */
    LABEL = 491,                   /* LABEL  */
    LAMBDA = 492,                  /* LAMBDA  */
    LANGUAGE = 493,                /* LANGUAGE  */
    LARGE_P = 494,                 /* LARGE_P  */
    LAST_P = 495,                  /* LAST_P  */
    LATERAL_P = 496,               /* LATERAL_P  */
    LEADING = 497,                 /* LEADING  */
    LEAKPROOF = 498,               /* LEAKPROOF  */
    LEFT = 499,                    /* LEFT  */
    LEVEL = 500,                   /* LEVEL  */
    LIKE = 501,                    /* LIKE  */
    LIMIT = 502,                   /* LIMIT  */
    LISTEN = 503,                  /* LISTEN  */
    LLM = 504,                     /* LLM  */
    LM = 505,                      /* LM  */
    LOAD = 506,                    /* LOAD  */
    LOCAL = 507,                   /* LOCAL  */
    LOCATION = 508,                /* LOCATION  */
    LOCK_P = 509,                  /* LOCK_P  */
    LOCKED = 510,                  /* LOCKED  */
    LOGGED = 511,                  /* LOGGED  */
    MACRO = 512,                   /* MACRO  */
    MAP = 513,                     /* MAP  */
    MAPPING = 514,                 /* MAPPING  */
    MATCH = 515,                   /* MATCH  */
    MATCHED = 516,                 /* MATCHED  */
    MATERIALIZED = 517,            /* MATERIALIZED  */
    MAXVALUE = 518,                /* MAXVALUE  */
    MERGE = 519,                   /* MERGE  */
    METHOD = 520,                  /* METHOD  */
    MICROSECOND_P = 521,           /* MICROSECOND_P  */
    MICROSECONDS_P = 522,          /* MICROSECONDS_P  */
    MILLENNIA_P = 523,             /* MILLENNIA_P  */
    MILLENNIUM_P = 524,            /* MILLENNIUM_P  */
    MILLISECOND_P = 525,           /* MILLISECOND_P  */
    MILLISECONDS_P = 526,          /* MILLISECONDS_P  */
    MINUTE_P = 527,                /* MINUTE_P  */
    MINUTES_P = 528,               /* MINUTES_P  */
    MINVALUE = 529,                /* MINVALUE  */
    MODE = 530,                    /* MODE  */
    MODEL = 531,                   /* MODEL  */
    MONTH_P = 532,                 /* MONTH_P  */
    MONTHS_P = 533,                /* MONTHS_P  */
    MOVE = 534,                    /* MOVE  */
    NAME_P = 535,                  /* NAME_P  */
    NAMES = 536,                   /* NAMES  */
    NATIONAL = 537,                /* NATIONAL  */
    NATURAL = 538,                 /* NATURAL  */
    NCHAR = 539,                   /* NCHAR  */
    NEW = 540,                     /* NEW  */
    NEXT = 541,                    /* NEXT  */
    NO = 542,                      /* NO  */
    NODES = 543,                   /* NODES  */
    NONE = 544,                    /* NONE  */
    NOT = 545,                     /* NOT  */
    NOTHING = 546,                 /* NOTHING  */
    NOTIFY = 547,                  /* NOTIFY  */
    NOTNULL = 548,                 /* NOTNULL  */
    NOWAIT = 549,                  /* NOWAIT  */
    NULL_P = 550,                  /* NULL_P  */
    NULLIF = 551,                  /* NULLIF  */
    NULLS_P = 552,                 /* NULLS_P  */
    NUMERIC = 553,                 /* NUMERIC  */
    OBJECT_P = 554,                /* OBJECT_P  */
    OF = 555,                      /* OF  */
    OFF = 556,                     /* OFF  */
    OFFSET = 557,                  /* OFFSET  */
    OIDS = 558,                    /* OIDS  */
    OLD = 559,                     /* OLD  */
    ON = 560,                      /* ON  */
    ONLY = 561,                    /* ONLY  */
    OPERATOR = 562,                /* OPERATOR  */
    OPTION = 563,                  /* OPTION  */
    OPTIONS = 564,                 /* OPTIONS  */
    OR = 565,                      /* OR  */
    ORDER = 566,                   /* ORDER  */
    ORDINALITY = 567,              /* ORDINALITY  */
    OTHERS = 568,                  /* OTHERS  */
    OUT_P = 569,                   /* OUT_P  */
    OUTER_P = 570,                 /* OUTER_P  */
    OUTPUT = 571,                  /* OUTPUT  */
    OVER = 572,                    /* OVER  */
    OVERLAPS = 573,                /* OVERLAPS  */
    OVERLAY = 574,                 /* OVERLAY  */
    OVERRIDING = 575,              /* OVERRIDING  */
    OWNED = 576,                   /* OWNED  */
    OWNER = 577,                   /* OWNER  */
    PARALLEL = 578,                /* PARALLEL  */
    PARSER = 579,                  /* PARSER  */
    PARTIAL = 580,                 /* PARTIAL  */
    PARTITION = 581,               /* PARTITION  */
    PARTITIONED = 582,             /* PARTITIONED  */
    PASSING = 583,                 /* PASSING  */
    PASSWORD = 584,                /* PASSWORD  */
    PATH = 585,                    /* PATH  */
    PERCENT = 586,                 /* PERCENT  */
    PERSISTENT = 587,              /* PERSISTENT  */
    PIVOT = 588,                   /* PIVOT  */
    PIVOT_LONGER = 589,            /* PIVOT_LONGER  */
    PIVOT_WIDER = 590,             /* PIVOT_WIDER  */
    PLACING = 591,                 /* PLACING  */
    PLANS = 592,                   /* PLANS  */
    POLICY = 593,                  /* POLICY  */
    POSITION = 594,                /* POSITION  */
    POSITIONAL = 595,              /* POSITIONAL  */
    PRAGMA_P = 596,                /* PRAGMA_P  */
    PRECEDING = 597,               /* PRECEDING  */
    PRECISION = 598,               /* PRECISION  */
    PREDICT = 599,                 /* PREDICT  */
    PREPARE = 600,                 /* PREPARE  */
    PREPARED = 601,                /* PREPARED  */
    PRESERVE = 602,                /* PRESERVE  */
    PRIMARY = 603,                 /* PRIMARY  */
    PRIOR = 604,                   /* PRIOR  */
    PRIVILEGES = 605,              /* PRIVILEGES  */
    PROCEDURAL = 606,              /* PROCEDURAL  */
    PROCEDURE = 607,               /* PROCEDURE  */
    PROGRAM = 608,                 /* PROGRAM  */
    PROMPT = 609,                  /* PROMPT  */
    PUBLICATION = 610,             /* PUBLICATION  */
    QUALIFY = 611,                 /* QUALIFY  */
    QUARTER_P = 612,               /* QUARTER_P  */
    QUARTERS_P = 613,              /* QUARTERS_P  */
    QUOTE = 614,                   /* QUOTE  */
    RANGE = 615,                   /* RANGE  */
    READ_P = 616,                  /* READ_P  */
    REAL = 617,                    /* REAL  */
    REASSIGN = 618,                /* REASSIGN  */
    RECHECK = 619,                 /* RECHECK  */
    RECURSIVE = 620,               /* RECURSIVE  */
    REF = 621,                     /* REF  */
    REFERENCES = 622,              /* REFERENCES  */
    REFERENCING = 623,             /* REFERENCING  */
    REFRESH = 624,                 /* REFRESH  */
    REINDEX = 625,                 /* REINDEX  */
    RELATIVE_P = 626,              /* RELATIVE_P  */
    RELEASE = 627,                 /* RELEASE  */
    RENAME = 628,                  /* RENAME  */
    REPEATABLE = 629,              /* REPEATABLE  */
    REPLACE = 630,                 /* REPLACE  */
    REPLICA = 631,                 /* REPLICA  */
    RESET = 632,                   /* RESET  */
    RESPECT_P = 633,               /* RESPECT_P  */
    RESTART = 634,                 /* RESTART  */
    RESTRICT = 635,                /* RESTRICT  */
    RETURNING = 636,               /* RETURNING  */
    RETURNS = 637,                 /* RETURNS  */
    REVOKE = 638,                  /* REVOKE  */
    RIGHT = 639,                   /* RIGHT  */
    ROLE = 640,                    /* ROLE  */
    ROLLBACK = 641,                /* ROLLBACK  */
    ROLLUP = 642,                  /* ROLLUP  */
    ROW = 643,                     /* ROW  */
    ROWS = 644,                    /* ROWS  */
    RULE = 645,                    /* RULE  */
    SAMPLE = 646,                  /* SAMPLE  */
    SAVEPOINT = 647,               /* SAVEPOINT  */
    SCHEMA = 648,                  /* SCHEMA  */
    SCHEMAS = 649,                 /* SCHEMAS  */
    SCOPE = 650,                   /* SCOPE  */
    SCROLL = 651,                  /* SCROLL  */
    SEARCH = 652,                  /* SEARCH  */
    SECOND_P = 653,                /* SECOND_P  */
    SECONDS_P = 654,               /* SECONDS_P  */
    SECRET = 655,                  /* SECRET  */
    SECURITY = 656,                /* SECURITY  */
    SELECT = 657,                  /* SELECT  */
    SEMI = 658,                    /* SEMI  */
    SEQUENCE = 659,                /* SEQUENCE  */
    SEQUENCES = 660,               /* SEQUENCES  */
    SERIALIZABLE = 661,            /* SERIALIZABLE  */
    SERVER = 662,                  /* SERVER  */
    SESSION = 663,                 /* SESSION  */
    SET = 664,                     /* SET  */
    SETOF = 665,                   /* SETOF  */
    SETS = 666,                    /* SETS  */
    SHARE = 667,                   /* SHARE  */
    SHOW = 668,                    /* SHOW  */
    SIMILAR = 669,                 /* SIMILAR  */
    SIMPLE = 670,                  /* SIMPLE  */
    SKIP = 671,                    /* SKIP  */
    SMALLINT = 672,                /* SMALLINT  */
    SNAPSHOT = 673,                /* SNAPSHOT  */
    SOME = 674,                    /* SOME  */
    SORTED = 675,                  /* SORTED  */
    SOURCE_P = 676,                /* SOURCE_P  */
    SQL_P = 677,                   /* SQL_P  */
    STABLE = 678,                  /* STABLE  */
    STANDALONE_P = 679,            /* STANDALONE_P  */
    START = 680,                   /* START  */
    STATEMENT = 681,               /* STATEMENT  */
    STATISTICS = 682,              /* STATISTICS  */
    STDIN = 683,                   /* STDIN  */
    STDOUT = 684,                  /* STDOUT  */
    STORAGE = 685,                 /* STORAGE  */
    STORED = 686,                  /* STORED  */
    STRICT_P = 687,                /* STRICT_P  */
    STRIP_P = 688,                 /* STRIP_P  */
    STRUCT = 689,                  /* STRUCT  */
    SUBSCRIPTION = 690,            /* SUBSCRIPTION  */
    SUBSTRING = 691,               /* SUBSTRING  */
    SUMMARIZE = 692,               /* SUMMARIZE  */
    SYMMETRIC = 693,               /* SYMMETRIC  */
    SYSID = 694,                   /* SYSID  */
    SYSTEM_P = 695,                /* SYSTEM_P  */
    TABLE = 696,                   /* TABLE  */
    TABLES = 697,                  /* TABLES  */
    TABLESAMPLE = 698,             /* TABLESAMPLE  */
    TABLESPACE = 699,              /* TABLESPACE  */
    TABULAR = 700,                 /* TABULAR  */
    TAKES = 701,                   /* TAKES  */
    TARGET_P = 702,                /* TARGET_P  */
    TEMP = 703,                    /* TEMP  */
    TEMPLATE = 704,                /* TEMPLATE  */
    TEMPORARY = 705,               /* TEMPORARY  */
    TEXT_P = 706,                  /* TEXT_P  */
    THEN = 707,                    /* THEN  */
    TIES = 708,                    /* TIES  */
    TIME = 709,                    /* TIME  */
    TIMESTAMP = 710,               /* TIMESTAMP  */
    TO = 711,                      /* TO  */
    TRAILING = 712,                /* TRAILING  */
    TRANSACTION = 713,             /* TRANSACTION  */
    TRANSFORM = 714,               /* TRANSFORM  */
    TREAT = 715,                   /* TREAT  */
    TRIGGER = 716,                 /* TRIGGER  */
    TRIM = 717,                    /* TRIM  */
    TRUE_P = 718,                  /* TRUE_P  */
    TRUNCATE = 719,                /* TRUNCATE  */
    TRUSTED = 720,                 /* TRUSTED  */
    TRY_CAST = 721,                /* TRY_CAST  */
    TYPE_P = 722,                  /* TYPE_P  */
    TYPES_P = 723,                 /* TYPES_P  */
    UNBOUNDED = 724,               /* UNBOUNDED  */
    UNCOMMITTED = 725,             /* UNCOMMITTED  */
    UNENCRYPTED = 726,             /* UNENCRYPTED  */
    UNION = 727,                   /* UNION  */
    UNIQUE = 728,                  /* UNIQUE  */
    UNKNOWN = 729,                 /* UNKNOWN  */
    UNLISTEN = 730,                /* UNLISTEN  */
    UNLOGGED = 731,                /* UNLOGGED  */
    UNPACK = 732,                  /* UNPACK  */
    UNPIVOT = 733,                 /* UNPIVOT  */
    UNTIL = 734,                   /* UNTIL  */
    UPDATE = 735,                  /* UPDATE  */
    USE_P = 736,                   /* USE_P  */
    USER = 737,                    /* USER  */
    USING = 738,                   /* USING  */
    VACUUM = 739,                  /* VACUUM  */
    VALID = 740,                   /* VALID  */
    VALIDATE = 741,                /* VALIDATE  */
    VALIDATOR = 742,               /* VALIDATOR  */
    VALUE_P = 743,                 /* VALUE_P  */
    VALUES = 744,                  /* VALUES  */
    VARCHAR = 745,                 /* VARCHAR  */
    VARIABLE_P = 746,              /* VARIABLE_P  */
    VARIADIC = 747,                /* VARIADIC  */
    VARYING = 748,                 /* VARYING  */
    VERBOSE = 749,                 /* VERBOSE  */
    VERSION_P = 750,               /* VERSION_P  */
    VIEW = 751,                    /* VIEW  */
    VIEWS = 752,                   /* VIEWS  */
    VIRTUAL = 753,                 /* VIRTUAL  */
    VOLATILE = 754,                /* VOLATILE  */
    WEEK_P = 755,                  /* WEEK_P  */
    WEEKS_P = 756,                 /* WEEKS_P  */
    WHEN = 757,                    /* WHEN  */
    WHERE = 758,                   /* WHERE  */
    WHITESPACE_P = 759,            /* WHITESPACE_P  */
    WINDOW = 760,                  /* WINDOW  */
    WITH = 761,                    /* WITH  */
    WITHIN = 762,                  /* WITHIN  */
    WITHOUT = 763,                 /* WITHOUT  */
    WORK = 764,                    /* WORK  */
    WRAPPER = 765,                 /* WRAPPER  */
    WRITE_P = 766,                 /* WRITE_P  */
    XML_P = 767,                   /* XML_P  */
    XMLATTRIBUTES = 768,           /* XMLATTRIBUTES  */
    XMLCONCAT = 769,               /* XMLCONCAT  */
    XMLELEMENT = 770,              /* XMLELEMENT  */
    XMLEXISTS = 771,               /* XMLEXISTS  */
    XMLFOREST = 772,               /* XMLFOREST  */
    XMLNAMESPACES = 773,           /* XMLNAMESPACES  */
    XMLPARSE = 774,                /* XMLPARSE  */
    XMLPI = 775,                   /* XMLPI  */
    XMLROOT = 776,                 /* XMLROOT  */
    XMLSERIALIZE = 777,            /* XMLSERIALIZE  */
    XMLTABLE = 778,                /* XMLTABLE  */
    YEAR_P = 779,                  /* YEAR_P  */
    YEARS_P = 780,                 /* YEARS_P  */
    YES_P = 781,                   /* YES_P  */
    ZONE = 782,                    /* ZONE  */
    NOT_LA = 783,                  /* NOT_LA  */
    NULLS_LA = 784,                /* NULLS_LA  */
    WITH_LA = 785,                 /* WITH_LA  */
    POSTFIXOP = 786,               /* POSTFIXOP  */
    UMINUS = 787                   /* UMINUS  */
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
	PGMergeAction mergeaction;

#line 648 "third_party/libpg_query/grammar/grammar_out.hpp"

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
