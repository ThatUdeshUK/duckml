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
    ERROR_P = 404,                 /* ERROR_P  */
    ESCAPE = 405,                  /* ESCAPE  */
    EVENT = 406,                   /* EVENT  */
    EXCEPT = 407,                  /* EXCEPT  */
    EXCLUDE = 408,                 /* EXCLUDE  */
    EXCLUDING = 409,               /* EXCLUDING  */
    EXCLUSIVE = 410,               /* EXCLUSIVE  */
    EXECUTE = 411,                 /* EXECUTE  */
    EXISTS = 412,                  /* EXISTS  */
    EXPLAIN = 413,                 /* EXPLAIN  */
    EXPORT_P = 414,                /* EXPORT_P  */
    EXPORT_STATE = 415,            /* EXPORT_STATE  */
    EXTENSION = 416,               /* EXTENSION  */
    EXTENSIONS = 417,              /* EXTENSIONS  */
    EXTERNAL = 418,                /* EXTERNAL  */
    EXTRACT = 419,                 /* EXTRACT  */
    FALSE_P = 420,                 /* FALSE_P  */
    FAMILY = 421,                  /* FAMILY  */
    FEATURES = 422,                /* FEATURES  */
    FETCH = 423,                   /* FETCH  */
    FILTER = 424,                  /* FILTER  */
    FIRST_P = 425,                 /* FIRST_P  */
    FLOAT_P = 426,                 /* FLOAT_P  */
    FOLLOWING = 427,               /* FOLLOWING  */
    FOR = 428,                     /* FOR  */
    FORCE = 429,                   /* FORCE  */
    FOREIGN = 430,                 /* FOREIGN  */
    FORWARD = 431,                 /* FORWARD  */
    FREEZE = 432,                  /* FREEZE  */
    FROM = 433,                    /* FROM  */
    FULL = 434,                    /* FULL  */
    FUNCTION = 435,                /* FUNCTION  */
    FUNCTIONS = 436,               /* FUNCTIONS  */
    GENERATED = 437,               /* GENERATED  */
    GLOB = 438,                    /* GLOB  */
    GLOBAL = 439,                  /* GLOBAL  */
    GNN = 440,                     /* GNN  */
    GRANT = 441,                   /* GRANT  */
    GRANTED = 442,                 /* GRANTED  */
    GROUP_P = 443,                 /* GROUP_P  */
    GROUPING = 444,                /* GROUPING  */
    GROUPING_ID = 445,             /* GROUPING_ID  */
    GROUPS = 446,                  /* GROUPS  */
    HANDLER = 447,                 /* HANDLER  */
    HAVING = 448,                  /* HAVING  */
    HEADER_P = 449,                /* HEADER_P  */
    HOLD = 450,                    /* HOLD  */
    HOUR_P = 451,                  /* HOUR_P  */
    HOURS_P = 452,                 /* HOURS_P  */
    IDENTITY_P = 453,              /* IDENTITY_P  */
    IF_P = 454,                    /* IF_P  */
    IGNORE_P = 455,                /* IGNORE_P  */
    ILIKE = 456,                   /* ILIKE  */
    IMMEDIATE = 457,               /* IMMEDIATE  */
    IMMUTABLE = 458,               /* IMMUTABLE  */
    IMPLICIT_P = 459,              /* IMPLICIT_P  */
    IMPORT_P = 460,                /* IMPORT_P  */
    IN_P = 461,                    /* IN_P  */
    INCLUDE_P = 462,               /* INCLUDE_P  */
    INCLUDING = 463,               /* INCLUDING  */
    INCREMENT = 464,               /* INCREMENT  */
    INDEX = 465,                   /* INDEX  */
    INDEXES = 466,                 /* INDEXES  */
    INHERIT = 467,                 /* INHERIT  */
    INHERITS = 468,                /* INHERITS  */
    INITIALLY = 469,               /* INITIALLY  */
    INLINE_P = 470,                /* INLINE_P  */
    INNER_P = 471,                 /* INNER_P  */
    INOUT = 472,                   /* INOUT  */
    INPUT_P = 473,                 /* INPUT_P  */
    INSENSITIVE = 474,             /* INSENSITIVE  */
    INSERT = 475,                  /* INSERT  */
    INSTALL = 476,                 /* INSTALL  */
    INSTEAD = 477,                 /* INSTEAD  */
    INT_P = 478,                   /* INT_P  */
    INTEGER = 479,                 /* INTEGER  */
    INTERSECT = 480,               /* INTERSECT  */
    INTERVAL = 481,                /* INTERVAL  */
    INTO = 482,                    /* INTO  */
    INVOKER = 483,                 /* INVOKER  */
    IS = 484,                      /* IS  */
    ISNULL = 485,                  /* ISNULL  */
    ISOLATION = 486,               /* ISOLATION  */
    JOIN = 487,                    /* JOIN  */
    JSON = 488,                    /* JSON  */
    KEY = 489,                     /* KEY  */
    LABEL = 490,                   /* LABEL  */
    LAMBDA = 491,                  /* LAMBDA  */
    LANGUAGE = 492,                /* LANGUAGE  */
    LARGE_P = 493,                 /* LARGE_P  */
    LAST_P = 494,                  /* LAST_P  */
    LATERAL_P = 495,               /* LATERAL_P  */
    LEADING = 496,                 /* LEADING  */
    LEAKPROOF = 497,               /* LEAKPROOF  */
    LEFT = 498,                    /* LEFT  */
    LEVEL = 499,                   /* LEVEL  */
    LIKE = 500,                    /* LIKE  */
    LIMIT = 501,                   /* LIMIT  */
    LISTEN = 502,                  /* LISTEN  */
    LLM = 503,                     /* LLM  */
    LM = 504,                      /* LM  */
    LOAD = 505,                    /* LOAD  */
    LOCAL = 506,                   /* LOCAL  */
    LOCATION = 507,                /* LOCATION  */
    LOCK_P = 508,                  /* LOCK_P  */
    LOCKED = 509,                  /* LOCKED  */
    LOGGED = 510,                  /* LOGGED  */
    MACRO = 511,                   /* MACRO  */
    MAP = 512,                     /* MAP  */
    MAPPING = 513,                 /* MAPPING  */
    MATCH = 514,                   /* MATCH  */
    MATCHED = 515,                 /* MATCHED  */
    MATERIALIZED = 516,            /* MATERIALIZED  */
    MAXVALUE = 517,                /* MAXVALUE  */
    MERGE = 518,                   /* MERGE  */
    METHOD = 519,                  /* METHOD  */
    MICROSECOND_P = 520,           /* MICROSECOND_P  */
    MICROSECONDS_P = 521,          /* MICROSECONDS_P  */
    MILLENNIA_P = 522,             /* MILLENNIA_P  */
    MILLENNIUM_P = 523,            /* MILLENNIUM_P  */
    MILLISECOND_P = 524,           /* MILLISECOND_P  */
    MILLISECONDS_P = 525,          /* MILLISECONDS_P  */
    MINUTE_P = 526,                /* MINUTE_P  */
    MINUTES_P = 527,               /* MINUTES_P  */
    MINVALUE = 528,                /* MINVALUE  */
    MODE = 529,                    /* MODE  */
    MODEL = 530,                   /* MODEL  */
    MONTH_P = 531,                 /* MONTH_P  */
    MONTHS_P = 532,                /* MONTHS_P  */
    MOVE = 533,                    /* MOVE  */
    NAME_P = 534,                  /* NAME_P  */
    NAMES = 535,                   /* NAMES  */
    NATIONAL = 536,                /* NATIONAL  */
    NATURAL = 537,                 /* NATURAL  */
    NCHAR = 538,                   /* NCHAR  */
    NEW = 539,                     /* NEW  */
    NEXT = 540,                    /* NEXT  */
    NO = 541,                      /* NO  */
    NODES = 542,                   /* NODES  */
    NONE = 543,                    /* NONE  */
    NOT = 544,                     /* NOT  */
    NOTHING = 545,                 /* NOTHING  */
    NOTIFY = 546,                  /* NOTIFY  */
    NOTNULL = 547,                 /* NOTNULL  */
    NOWAIT = 548,                  /* NOWAIT  */
    NULL_P = 549,                  /* NULL_P  */
    NULLIF = 550,                  /* NULLIF  */
    NULLS_P = 551,                 /* NULLS_P  */
    NUMERIC = 552,                 /* NUMERIC  */
    OBJECT_P = 553,                /* OBJECT_P  */
    OF = 554,                      /* OF  */
    OFF = 555,                     /* OFF  */
    OFFSET = 556,                  /* OFFSET  */
    OIDS = 557,                    /* OIDS  */
    OLD = 558,                     /* OLD  */
    ON = 559,                      /* ON  */
    ONLY = 560,                    /* ONLY  */
    OPERATOR = 561,                /* OPERATOR  */
    OPTION = 562,                  /* OPTION  */
    OPTIONS = 563,                 /* OPTIONS  */
    OR = 564,                      /* OR  */
    ORDER = 565,                   /* ORDER  */
    ORDINALITY = 566,              /* ORDINALITY  */
    OTHERS = 567,                  /* OTHERS  */
    OUT_P = 568,                   /* OUT_P  */
    OUTER_P = 569,                 /* OUTER_P  */
    OUTPUT = 570,                  /* OUTPUT  */
    OVER = 571,                    /* OVER  */
    OVERLAPS = 572,                /* OVERLAPS  */
    OVERLAY = 573,                 /* OVERLAY  */
    OVERRIDING = 574,              /* OVERRIDING  */
    OWNED = 575,                   /* OWNED  */
    OWNER = 576,                   /* OWNER  */
    PARALLEL = 577,                /* PARALLEL  */
    PARSER = 578,                  /* PARSER  */
    PARTIAL = 579,                 /* PARTIAL  */
    PARTITION = 580,               /* PARTITION  */
    PARTITIONED = 581,             /* PARTITIONED  */
    PASSING = 582,                 /* PASSING  */
    PASSWORD = 583,                /* PASSWORD  */
    PATH = 584,                    /* PATH  */
    PERCENT = 585,                 /* PERCENT  */
    PERSISTENT = 586,              /* PERSISTENT  */
    PIVOT = 587,                   /* PIVOT  */
    PIVOT_LONGER = 588,            /* PIVOT_LONGER  */
    PIVOT_WIDER = 589,             /* PIVOT_WIDER  */
    PLACING = 590,                 /* PLACING  */
    PLANS = 591,                   /* PLANS  */
    POLICY = 592,                  /* POLICY  */
    POSITION = 593,                /* POSITION  */
    POSITIONAL = 594,              /* POSITIONAL  */
    PRAGMA_P = 595,                /* PRAGMA_P  */
    PRECEDING = 596,               /* PRECEDING  */
    PRECISION = 597,               /* PRECISION  */
    PREDICT = 598,                 /* PREDICT  */
    PREPARE = 599,                 /* PREPARE  */
    PREPARED = 600,                /* PREPARED  */
    PRESERVE = 601,                /* PRESERVE  */
    PRIMARY = 602,                 /* PRIMARY  */
    PRIOR = 603,                   /* PRIOR  */
    PRIVILEGES = 604,              /* PRIVILEGES  */
    PROCEDURAL = 605,              /* PROCEDURAL  */
    PROCEDURE = 606,               /* PROCEDURE  */
    PROGRAM = 607,                 /* PROGRAM  */
    PROMPT = 608,                  /* PROMPT  */
    PUBLICATION = 609,             /* PUBLICATION  */
    QUALIFY = 610,                 /* QUALIFY  */
    QUARTER_P = 611,               /* QUARTER_P  */
    QUARTERS_P = 612,              /* QUARTERS_P  */
    QUOTE = 613,                   /* QUOTE  */
    RANGE = 614,                   /* RANGE  */
    READ_P = 615,                  /* READ_P  */
    REAL = 616,                    /* REAL  */
    REASSIGN = 617,                /* REASSIGN  */
    RECHECK = 618,                 /* RECHECK  */
    RECURSIVE = 619,               /* RECURSIVE  */
    REF = 620,                     /* REF  */
    REFERENCES = 621,              /* REFERENCES  */
    REFERENCING = 622,             /* REFERENCING  */
    REFRESH = 623,                 /* REFRESH  */
    REINDEX = 624,                 /* REINDEX  */
    RELATIVE_P = 625,              /* RELATIVE_P  */
    RELEASE = 626,                 /* RELEASE  */
    RENAME = 627,                  /* RENAME  */
    REPEATABLE = 628,              /* REPEATABLE  */
    REPLACE = 629,                 /* REPLACE  */
    REPLICA = 630,                 /* REPLICA  */
    RESET = 631,                   /* RESET  */
    RESPECT_P = 632,               /* RESPECT_P  */
    RESTART = 633,                 /* RESTART  */
    RESTRICT = 634,                /* RESTRICT  */
    RETURNING = 635,               /* RETURNING  */
    RETURNS = 636,                 /* RETURNS  */
    REVOKE = 637,                  /* REVOKE  */
    RIGHT = 638,                   /* RIGHT  */
    ROLE = 639,                    /* ROLE  */
    ROLLBACK = 640,                /* ROLLBACK  */
    ROLLUP = 641,                  /* ROLLUP  */
    ROW = 642,                     /* ROW  */
    ROWS = 643,                    /* ROWS  */
    RULE = 644,                    /* RULE  */
    SAMPLE = 645,                  /* SAMPLE  */
    SAVEPOINT = 646,               /* SAVEPOINT  */
    SCHEMA = 647,                  /* SCHEMA  */
    SCHEMAS = 648,                 /* SCHEMAS  */
    SCOPE = 649,                   /* SCOPE  */
    SCROLL = 650,                  /* SCROLL  */
    SEARCH = 651,                  /* SEARCH  */
    SECOND_P = 652,                /* SECOND_P  */
    SECONDS_P = 653,               /* SECONDS_P  */
    SECRET = 654,                  /* SECRET  */
    SECURITY = 655,                /* SECURITY  */
    SELECT = 656,                  /* SELECT  */
    SEMI = 657,                    /* SEMI  */
    SEQUENCE = 658,                /* SEQUENCE  */
    SEQUENCES = 659,               /* SEQUENCES  */
    SERIALIZABLE = 660,            /* SERIALIZABLE  */
    SERVER = 661,                  /* SERVER  */
    SESSION = 662,                 /* SESSION  */
    SET = 663,                     /* SET  */
    SETOF = 664,                   /* SETOF  */
    SETS = 665,                    /* SETS  */
    SHARE = 666,                   /* SHARE  */
    SHOW = 667,                    /* SHOW  */
    SIMILAR = 668,                 /* SIMILAR  */
    SIMPLE = 669,                  /* SIMPLE  */
    SKIP = 670,                    /* SKIP  */
    SMALLINT = 671,                /* SMALLINT  */
    SNAPSHOT = 672,                /* SNAPSHOT  */
    SOME = 673,                    /* SOME  */
    SORTED = 674,                  /* SORTED  */
    SOURCE_P = 675,                /* SOURCE_P  */
    SQL_P = 676,                   /* SQL_P  */
    STABLE = 677,                  /* STABLE  */
    STANDALONE_P = 678,            /* STANDALONE_P  */
    START = 679,                   /* START  */
    STATEMENT = 680,               /* STATEMENT  */
    STATISTICS = 681,              /* STATISTICS  */
    STDIN = 682,                   /* STDIN  */
    STDOUT = 683,                  /* STDOUT  */
    STORAGE = 684,                 /* STORAGE  */
    STORED = 685,                  /* STORED  */
    STRICT_P = 686,                /* STRICT_P  */
    STRIP_P = 687,                 /* STRIP_P  */
    STRUCT = 688,                  /* STRUCT  */
    SUBSCRIPTION = 689,            /* SUBSCRIPTION  */
    SUBSTRING = 690,               /* SUBSTRING  */
    SUMMARIZE = 691,               /* SUMMARIZE  */
    SYMMETRIC = 692,               /* SYMMETRIC  */
    SYSID = 693,                   /* SYSID  */
    SYSTEM_P = 694,                /* SYSTEM_P  */
    TABLE = 695,                   /* TABLE  */
    TABLES = 696,                  /* TABLES  */
    TABLESAMPLE = 697,             /* TABLESAMPLE  */
    TABLESPACE = 698,              /* TABLESPACE  */
    TABULAR = 699,                 /* TABULAR  */
    TAKES = 700,                   /* TAKES  */
    TARGET_P = 701,                /* TARGET_P  */
    TEMP = 702,                    /* TEMP  */
    TEMPLATE = 703,                /* TEMPLATE  */
    TEMPORARY = 704,               /* TEMPORARY  */
    TEXT_P = 705,                  /* TEXT_P  */
    THEN = 706,                    /* THEN  */
    TIES = 707,                    /* TIES  */
    TIME = 708,                    /* TIME  */
    TIMESTAMP = 709,               /* TIMESTAMP  */
    TO = 710,                      /* TO  */
    TRAILING = 711,                /* TRAILING  */
    TRANSACTION = 712,             /* TRANSACTION  */
    TRANSFORM = 713,               /* TRANSFORM  */
    TREAT = 714,                   /* TREAT  */
    TRIGGER = 715,                 /* TRIGGER  */
    TRIM = 716,                    /* TRIM  */
    TRUE_P = 717,                  /* TRUE_P  */
    TRUNCATE = 718,                /* TRUNCATE  */
    TRUSTED = 719,                 /* TRUSTED  */
    TRY_CAST = 720,                /* TRY_CAST  */
    TYPE_P = 721,                  /* TYPE_P  */
    TYPES_P = 722,                 /* TYPES_P  */
    UNBOUNDED = 723,               /* UNBOUNDED  */
    UNCOMMITTED = 724,             /* UNCOMMITTED  */
    UNENCRYPTED = 725,             /* UNENCRYPTED  */
    UNION = 726,                   /* UNION  */
    UNIQUE = 727,                  /* UNIQUE  */
    UNKNOWN = 728,                 /* UNKNOWN  */
    UNLISTEN = 729,                /* UNLISTEN  */
    UNLOGGED = 730,                /* UNLOGGED  */
    UNPACK = 731,                  /* UNPACK  */
    UNPIVOT = 732,                 /* UNPIVOT  */
    UNTIL = 733,                   /* UNTIL  */
    UPDATE = 734,                  /* UPDATE  */
    USE_P = 735,                   /* USE_P  */
    USER = 736,                    /* USER  */
    USING = 737,                   /* USING  */
    VACUUM = 738,                  /* VACUUM  */
    VALID = 739,                   /* VALID  */
    VALIDATE = 740,                /* VALIDATE  */
    VALIDATOR = 741,               /* VALIDATOR  */
    VALUE_P = 742,                 /* VALUE_P  */
    VALUES = 743,                  /* VALUES  */
    VARCHAR = 744,                 /* VARCHAR  */
    VARIABLE_P = 745,              /* VARIABLE_P  */
    VARIADIC = 746,                /* VARIADIC  */
    VARYING = 747,                 /* VARYING  */
    VERBOSE = 748,                 /* VERBOSE  */
    VERSION_P = 749,               /* VERSION_P  */
    VIEW = 750,                    /* VIEW  */
    VIEWS = 751,                   /* VIEWS  */
    VIRTUAL = 752,                 /* VIRTUAL  */
    VOLATILE = 753,                /* VOLATILE  */
    WEEK_P = 754,                  /* WEEK_P  */
    WEEKS_P = 755,                 /* WEEKS_P  */
    WHEN = 756,                    /* WHEN  */
    WHERE = 757,                   /* WHERE  */
    WHITESPACE_P = 758,            /* WHITESPACE_P  */
    WINDOW = 759,                  /* WINDOW  */
    WITH = 760,                    /* WITH  */
    WITHIN = 761,                  /* WITHIN  */
    WITHOUT = 762,                 /* WITHOUT  */
    WORK = 763,                    /* WORK  */
    WRAPPER = 764,                 /* WRAPPER  */
    WRITE_P = 765,                 /* WRITE_P  */
    XML_P = 766,                   /* XML_P  */
    XMLATTRIBUTES = 767,           /* XMLATTRIBUTES  */
    XMLCONCAT = 768,               /* XMLCONCAT  */
    XMLELEMENT = 769,              /* XMLELEMENT  */
    XMLEXISTS = 770,               /* XMLEXISTS  */
    XMLFOREST = 771,               /* XMLFOREST  */
    XMLNAMESPACES = 772,           /* XMLNAMESPACES  */
    XMLPARSE = 773,                /* XMLPARSE  */
    XMLPI = 774,                   /* XMLPI  */
    XMLROOT = 775,                 /* XMLROOT  */
    XMLSERIALIZE = 776,            /* XMLSERIALIZE  */
    XMLTABLE = 777,                /* XMLTABLE  */
    YEAR_P = 778,                  /* YEAR_P  */
    YEARS_P = 779,                 /* YEARS_P  */
    YES_P = 780,                   /* YES_P  */
    ZONE = 781,                    /* ZONE  */
    NOT_LA = 782,                  /* NOT_LA  */
    NULLS_LA = 783,                /* NULLS_LA  */
    WITH_LA = 784,                 /* WITH_LA  */
    POSTFIXOP = 785,               /* POSTFIXOP  */
    UMINUS = 786                   /* UMINUS  */
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

#line 647 "third_party/libpg_query/grammar/grammar_out.hpp"

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
