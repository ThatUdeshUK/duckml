/*****************************************************************************
 *
 *		QUERY: CREATE EMBEDDING
 *
 * Creates a vector embedding index on a table column using the specified
 * embedding model, with an optional vector size parameter.
 *****************************************************************************/
CreateEmbeddingStmt:
			CREATE_P EMBEDDING ColId ON qualified_name '(' ColId ')' USING qualified_name opt_embedding_size
				{
					PGCreateEmbeddingStmt *n = makeNode(PGCreateEmbeddingStmt);
					n->embeddingName = $3;
					n->relation = $5;
					n->column = $7;
					n->embeddingModel = $10;
					n->embeddingSize = $11;
					n->onconflict = PG_ERROR_ON_CONFLICT;
					$$ = (PGNode *)n;
				}
			| CREATE_P EMBEDDING IF_P NOT EXISTS ColId ON qualified_name '(' ColId ')' USING qualified_name opt_embedding_size
				{
					PGCreateEmbeddingStmt *n = makeNode(PGCreateEmbeddingStmt);
					n->embeddingName = $6;
					n->relation = $8;
					n->column = $10;
					n->embeddingModel = $13;
					n->embeddingSize = $14;
					n->onconflict = PG_IGNORE_ON_CONFLICT;
					$$ = (PGNode *)n;
				}
		;


opt_embedding_size:
			SIZE Iconst							{ $$ = $2; }
			| /*EMPTY*/							{ $$ = 0; }
		;
