#ifndef __DT_TYPES_H
#define __DT_TYPES_H

typedef struct _dt_rec_type { 
  int model_idx;
  int tree_idx;
  int node_idx;
  int lchild_idx;
  int rchild_idx;
  int feature_idx;
  float threshold;
  int nneg;
  int npos;
} DT_REC_TYPE;

typedef struct _rf_rec_type { 
  int tree_idx;
  int dt_lb; // lower bound (inclusive) into dt structure
  int dt_ub; // upper bound (exclusive) into dt structure
  int nneg; // these will get set at run time 
  int npos; // these will get set at run time 
  int model_idx;
} RF_REC_TYPE;

typedef struct _mdl_rec_type { 
  int model_idx;
  int rf_lb; // lower bound (inclusive) into rf structure
  int rf_ub; // upper bound (exclusive) into rf structure
  float prob; // will get set at run time 
} MDL_REC_TYPE;

#endif