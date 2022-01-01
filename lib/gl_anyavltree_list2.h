/* Sequential list data type implemented by a binary tree.
   Copyright (C) 2006-2007, 2009-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2006.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Common code of gl_avltree_list.c and gl_avltreehash_list.c.  */

/* -------------------------- gl_list_t Data Type -------------------------- */

/* Creates a subtree for count >= 1 elements.
   Its height is h where 2^(h-1) <= count <= 2^h - 1.
   Returns NULL upon out-of-memory.  */
static gl_list_node_t
create_subtree_with_contents (size_t count, const void **contents)
{
  size_t half1 = (count - 1) / 2;
  size_t half2 = count / 2;
  /* Note: half1 + half2 = count - 1.  */
  gl_list_node_t node =
    (struct gl_list_node_impl *) malloc (sizeof (struct gl_list_node_impl));
  if (node == NULL)
    return NULL;

  if (half1 > 0)
    {
      node->left = create_subtree_with_contents (half1, contents);
      if (node->left == NULL)
        goto fail1;
      node->left->parent = node;
    }
  else
    node->left = NULL;

  node->value = contents[half1];

  if (half2 > 0)
    {
      node->right = create_subtree_with_contents (half2, contents + half1 + 1);
      if (node->right == NULL)
        goto fail2;
      node->right->parent = node;
    }
  else
    node->right = NULL;

  /* balance is 0, except when count is a power of two and > 1.
     Reason: half1 <= half2 <= half1 + 1, and the two branches can have
     different heights only if half1 = 2^h - 1 and half2 = 2^h; in this
     case, count = half1 + half2 + 1 = 2^(h+1).  */
  node->balance = (count > 1 && (count & (count - 1)) == 0 ? 1 : 0);

  node->branch_size = count;

  return node;

 fail2:
  if (node->left != NULL)
    free_subtree (node->left);
 fail1:
  free (node);
  return NULL;
}

static gl_list_t
gl_tree_nx_create (gl_list_implementation_t implementation,
                   gl_listelement_equals_fn equals_fn,
                   gl_listelement_hashcode_fn hashcode_fn,
                   gl_listelement_dispose_fn dispose_fn,
                   bool allow_duplicates,
                   size_t count, const void **contents)
{
  struct gl_list_impl *list =
    (struct gl_list_impl *) malloc (sizeof (struct gl_list_impl));

  if (list == NULL)
    return NULL;

  list->base.vtable = implementation;
  list->base.equals_fn = equals_fn;
  list->base.hashcode_fn = hashcode_fn;
  list->base.dispose_fn = dispose_fn;
  list->base.allow_duplicates = allow_duplicates;
#if WITH_HASHTABLE
  {
    size_t estimate = xsum (count, count / 2); /* 1.5 * count */
    if (estimate < 10)
      estimate = 10;
    list->table_size = next_prime (estimate);
    if (size_overflow_p (xtimes (list->table_size, sizeof (gl_hash_entry_t))))
      goto fail1;
    list->table =
      (gl_hash_entry_t *) calloc (list->table_size, sizeof (gl_hash_entry_t));
    if (list->table == NULL)
      goto fail1;
  }
#endif
  if (count > 0)
    {
      list->root = create_subtree_with_contents (count, contents);
      if (list->root == NULL)
        goto fail2;
      list->root->parent = NULL;

#if WITH_HASHTABLE
      /* Now that the tree is built, node_position() works.  Now we can
         add the nodes to the hash table.  */
      if (add_nodes_to_buckets (list) < 0)
        goto fail3;
#endif
    }
  else
    list->root = NULL;

  return list;

#if WITH_HASHTABLE
 fail3:
  free_subtree (list->root);
#endif
 fail2:
#if WITH_HASHTABLE
  free (list->table);
 fail1:
#endif
  free (list);
  return NULL;
}

/* Ensures the tree is balanced, after an insertion or deletion operation.
   The height of NODE is incremented by HEIGHT_DIFF (1 or -1).
   PARENT = NODE->parent.  (NODE can also be NULL.  But PARENT is non-NULL.)
   Rotation operations are performed starting at PARENT (not NODE itself!).  */
static void
rebalance (gl_list_t list,
           gl_list_node_t node, int height_diff, gl_list_node_t parent)
{
  for (;;)
    {
      gl_list_node_t child;
      int previous_balance;
      int balance_diff;
      gl_list_node_t nodeleft;
      gl_list_node_t noderight;

      child = node;
      node = parent;

      previous_balance = node->balance;

      /* The balance of NODE is incremented by BALANCE_DIFF: +1 if the right
         branch's height has increased by 1 or the left branch's height has
         decreased by 1, -1 if the right branch's height has decreased by 1 or
         the left branch's height has increased by 1, 0 if no height change.  */
      if (node->left != NULL || node->right != NULL)
        balance_diff = (child == node->right ? height_diff : -height_diff);
      else
        /* Special case where above formula doesn't work, because the caller
           didn't tell whether node's left or right branch shrunk from height 1
           to NULL.  */
        balance_diff = - previous_balance;

      node->balance += balance_diff;
      if (balance_diff == previous_balance)
        {
          /* node->balance is outside the range [-1,1].  Must rotate.  */
          gl_list_node_t *nodep;

          if (node->parent == NULL)
            /* node == list->root */
            nodep = &list->root;
          else if (node->parent->left == node)
            nodep = &node->parent->left;
          else if (node->parent->right == node)
            nodep = &node->parent->right;
          else
            abort ();

          nodeleft = node->left;
          noderight = node->right;

          if (balance_diff < 0)
            {
              /* node->balance = -2.  The subtree is heavier on the left side.
                 Rotate from left to right:

                                  *
                                /   \
                             h+2      h
               */
              gl_list_node_t nodeleftleft = nodeleft->left;
              gl_list_node_t nodeleftright = nodeleft->right;
              if (nodeleft->balance <= 0)
                {
                  /*
                              *                    h+2|h+3
                            /   \                  /    \
                         h+2      h      -->      /   h+1|h+2
                         / \                      |    /    \
                       h+1 h|h+1                 h+1  h|h+1  h
                   */
                  node->left = nodeleftright;
                  nodeleft->right = node;

                  nodeleft->parent = node->parent;
                  node->parent = nodeleft;
                  if (nodeleftright != NULL)
                    nodeleftright->parent = node;

                  nodeleft->balance += 1;
                  node->balance = - nodeleft->balance;

                  node->branch_size =
                    (nodeleftright != NULL ? nodeleftright->branch_size : 0)
                    + 1 + (noderight != NULL ? noderight->branch_size : 0);
                  nodeleft->branch_size =
                    nodeleftleft->branch_size + 1 + node->branch_size;

                  *nodep = nodeleft;
                  height_diff = (height_diff < 0
                                 ? /* noderight's height had been decremented from
                                      h+1 to h.  The subtree's height changes from
                                      h+3 to h+2|h+3.  */
                                   nodeleft->balance - 1
                                 : /* nodeleft's height had been incremented from
                                      h+1 to h+2.  The subtree's height changes from
                                      h+2 to h+2|h+3.  */
                                   nodeleft->balance);
                }
              else
                {
                  /*
                            *                     h+2
                          /   \                 /     \
                       h+2      h      -->    h+1     h+1
                       / \                    / \     / \
                      h  h+1                 h   L   R   h
                         / \
                        L   R

                   */
                  gl_list_node_t L = nodeleft->right = nodeleftright->left;
                  gl_list_node_t R = node->left = nodeleftright->right;
                  nodeleftright->left = nodeleft;
                  nodeleftright->right = node;

                  nodeleftright->parent = node->parent;
                  if (L != NULL)
                    L->parent = nodeleft;
                  if (R != NULL)
                    R->parent = node;
                  nodeleft->parent = nodeleftright;
                  node->parent = nodeleftright;

                  nodeleft->balance = (nodeleftright->balance > 0 ? -1 : 0);
                  node->balance = (nodeleftright->balance < 0 ? 1 : 0);
                  nodeleftright->balance = 0;

                  nodeleft->branch_size =
                    (nodeleft->left != NULL ? nodeleft->left->branch_size : 0)
                    + 1 + (nodeleft->right != NULL ? nodeleft->right->branch_size : 0);
                  node->branch_size =
                    (node->left != NULL ? node->left->branch_size : 0)
                    + 1 + (node->right != NULL ? node->right->branch_size : 0);
                  nodeleftright->branch_size =
                    nodeleft->branch_size + 1 + node->branch_size;

                  *nodep = nodeleftright;
                  height_diff = (height_diff < 0
                                 ? /* noderight's height had been decremented from
                                      h+1 to h.  The subtree's height changes from
                                      h+3 to h+2.  */
                                   -1
                                 : /* nodeleft's height had been incremented from
                                      h+1 to h+2.  The subtree's height changes from
                                      h+2 to h+2.  */
                                   0);
                }
            }
          else
            {
              /* node->balance = 2.  The subtree is heavier on the right side.
                 Rotate from right to left:

                                  *
                                /   \
                              h      h+2
               */
              gl_list_node_t noderightleft = noderight->left;
              gl_list_node_t noderightright = noderight->right;
              if (noderight->balance >= 0)
                {
                  /*
                              *                    h+2|h+3
                            /   \                   /    \
                          h      h+2     -->    h+1|h+2   \
                                 / \            /    \    |
                             h|h+1 h+1         h   h|h+1 h+1
                   */
                  node->right = noderightleft;
                  noderight->left = node;

                  noderight->parent = node->parent;
                  node->parent = noderight;
                  if (noderightleft != NULL)
                    noderightleft->parent = node;

                  noderight->balance -= 1;
                  node->balance = - noderight->balance;

                  node->branch_size =
                    (nodeleft != NULL ? nodeleft->branch_size : 0)
                    + 1 + (noderightleft != NULL ? noderightleft->branch_size : 0);
                  noderight->branch_size =
                    node->branch_size + 1 + noderightright->branch_size;

                  *nodep = noderight;
                  height_diff = (height_diff < 0
                                 ? /* nodeleft's height had been decremented from
                                      h+1 to h.  The subtree's height changes from
                                      h+3 to h+2|h+3.  */
                                   - noderight->balance - 1
                                 : /* noderight's height had been incremented from
                                      h+1 to h+2.  The subtree's height changes from
                                      h+2 to h+2|h+3.  */
                                   - noderight->balance);
                }
              else
                {
                  /*
                            *                    h+2
                          /   \                /     \
                        h      h+2    -->    h+1     h+1
                               / \           / \     / \
                             h+1  h         h   L   R   h
                             / \
                            L   R

                   */
                  gl_list_node_t L = node->right = noderightleft->left;
                  gl_list_node_t R = noderight->left = noderightleft->right;
                  noderightleft->left = node;
                  noderightleft->right = noderight;

                  noderightleft->parent = node->parent;
                  if (L != NULL)
                    L->parent = node;
                  if (R != NULL)
                    R->parent = noderight;
                  node->parent = noderightleft;
                  noderight->parent = noderightleft;

                  node->balance = (noderightleft->balance > 0 ? -1 : 0);
                  noderight->balance = (noderightleft->balance < 0 ? 1 : 0);
                  noderightleft->balance = 0;

                  node->branch_size =
                    (node->left != NULL ? node->left->branch_size : 0)
                    + 1 + (node->right != NULL ? node->right->branch_size : 0);
                  noderight->branch_size =
                    (noderight->left != NULL ? noderight->left->branch_size : 0)
                    + 1 + (noderight->right != NULL ? noderight->right->branch_size : 0);
                  noderightleft->branch_size =
                    node->branch_size + 1 + noderight->branch_size;

                  *nodep = noderightleft;
                  height_diff = (height_diff < 0
                                 ? /* nodeleft's height had been decremented from
                                      h+1 to h.  The subtree's height changes from
                                      h+3 to h+2.  */
                                   -1
                                 : /* noderight's height had been incremented from
                                      h+1 to h+2.  The subtree's height changes from
                                      h+2 to h+2.  */
                                   0);
                }
            }
          node = *nodep;
        }
      else
        {
          /* No rotation needed.  Only propagation of the height change to the
             next higher level.  */
          if (height_diff < 0)
            height_diff = (previous_balance == 0 ? 0 : -1);
          else
            height_diff = (node->balance == 0 ? 0 : 1);
        }

      if (height_diff == 0)
        break;

      parent = node->parent;
      if (parent == NULL)
        break;
    }
}

static void
gl_tree_remove_node_from_tree (gl_list_t list, gl_list_node_t node)
{
  gl_list_node_t parent = node->parent;

  if (node->left == NULL)
    {
      /* Replace node with node->right.  */
      gl_list_node_t child = node->right;

      if (child != NULL)
        child->parent = parent;
      if (parent == NULL)
        list->root = child;
      else
        {
          if (parent->left == node)
            parent->left = child;
          else /* parent->right == node */
            parent->right = child;

          /* Update branch_size fields of the parent nodes.  */
          {
            gl_list_node_t p;

            for (p = parent; p != NULL; p = p->parent)
              p->branch_size--;
          }

          rebalance (list, child, -1, parent);
        }
    }
  else if (node->right == NULL)
    {
      /* It is not absolutely necessary to treat this case.  But the more
         general case below is more complicated, hence slower.  */
      /* Replace node with node->left.  */
      gl_list_node_t child = node->left;

      child->parent = parent;
      if (parent == NULL)
        list->root = child;
      else
        {
          if (parent->left == node)
            parent->left = child;
          else /* parent->right == node */
            parent->right = child;

          /* Update branch_size fields of the parent nodes.  */
          {
            gl_list_node_t p;

            for (p = parent; p != NULL; p = p->parent)
              p->branch_size--;
          }

          rebalance (list, child, -1, parent);
        }
    }
  else
    {
      /* Replace node with the rightmost element of the node->left subtree.  */
      gl_list_node_t subst;
      gl_list_node_t subst_parent;
      gl_list_node_t child;

      for (subst = node->left; subst->right != NULL; )
        subst = subst->right;

      subst_parent = subst->parent;

      child = subst->left;

      /* The case subst_parent == node is special:  If we do nothing special,
         we get confusion about node->left, subst->left and child->parent.
           subst_parent == node
           <==> The 'for' loop above terminated immediately.
           <==> subst == subst_parent->left
                [otherwise subst == subst_parent->right]
         In this case, we would need to first set
           child->parent = node; node->left = child;
         and later - when we copy subst into node's position - again
           child->parent = subst; subst->left = child;
         Altogether a no-op.  */
      if (subst_parent != node)
        {
          if (child != NULL)
            child->parent = subst_parent;
          subst_parent->right = child;
        }

      /* Update branch_size fields of the parent nodes.  */
      {
        gl_list_node_t p;

        for (p = subst_parent; p != NULL; p = p->parent)
          p->branch_size--;
      }

      /* Copy subst into node's position.
         (This is safer than to copy subst's value into node, keep node in
         place, and free subst.)  */
      if (subst_parent != node)
        {
          subst->left = node->left;
          subst->left->parent = subst;
        }
      subst->right = node->right;
      subst->right->parent = subst;
      subst->balance = node->balance;
      subst->branch_size = node->branch_size;
      subst->parent = parent;
      if (parent == NULL)
        list->root = subst;
      else if (parent->left == node)
        parent->left = subst;
      else /* parent->right == node */
        parent->right = subst;

      /* Rebalancing starts at child's parent, that is subst_parent -
         except when subst_parent == node.  In this case, we need to use
         its replacement, subst.  */
      rebalance (list, child, -1, subst_parent != node ? subst_parent : subst);
    }
}

static gl_list_node_t
gl_tree_nx_add_first (gl_list_t list, const void *elt)
{
  /* Create new node.  */
  gl_list_node_t new_node =
    (struct gl_list_node_impl *) malloc (sizeof (struct gl_list_node_impl));

  if (new_node == NULL)
    return NULL;

  new_node->left = NULL;
  new_node->right = NULL;
  new_node->balance = 0;
  new_node->branch_size = 1;
  new_node->value = elt;
#if WITH_HASHTABLE
  new_node->h.hashcode =
    (list->base.hashcode_fn != NULL
     ? list->base.hashcode_fn (new_node->value)
     : (size_t)(uintptr_t) new_node->value);
#endif

  /* Add it to the tree.  */
  if (list->root == NULL)
    {
      list->root = new_node;
      new_node->parent = NULL;
    }
  else
    {
      gl_list_node_t node;

      for (node = list->root; node->left != NULL; )
        node = node->left;

      node->left = new_node;
      new_node->parent = node;
      node->balance--;

      /* Update branch_size fields of the parent nodes.  */
      {
        gl_list_node_t p;

        for (p = node; p != NULL; p = p->parent)
          p->branch_size++;
      }

      /* Rebalance.  */
      if (node->right == NULL && node->parent != NULL)
        rebalance (list, node, 1, node->parent);
    }

#if WITH_HASHTABLE
  /* Add node to the hash table.
     Note that this is only possible _after_ the node has been added to the
     tree structure, because add_to_bucket() uses node_position().  */
  if (add_to_bucket (list, new_node) < 0)
    {
      gl_tree_remove_node_from_tree (list, new_node);
      free (new_node);
      return NULL;
    }
  hash_resize_after_add (list);
#endif

  return new_node;
}

static gl_list_node_t
gl_tree_nx_add_last (gl_list_t list, const void *elt)
{
  /* Create new node.  */
  gl_list_node_t new_node =
    (struct gl_list_node_impl *) malloc (sizeof (struct gl_list_node_impl));

  if (new_node == NULL)
    return NULL;

  new_node->left = NULL;
  new_node->right = NULL;
  new_node->balance = 0;
  new_node->branch_size = 1;
  new_node->value = elt;
#if WITH_HASHTABLE
  new_node->h.hashcode =
    (list->base.hashcode_fn != NULL
     ? list->base.hashcode_fn (new_node->value)
     : (size_t)(uintptr_t) new_node->value);
#endif

  /* Add it to the tree.  */
  if (list->root == NULL)
    {
      list->root = new_node;
      new_node->parent = NULL;
    }
  else
    {
      gl_list_node_t node;

      for (node = list->root; node->right != NULL; )
        node = node->right;

      node->right = new_node;
      new_node->parent = node;
      node->balance++;

      /* Update branch_size fields of the parent nodes.  */
      {
        gl_list_node_t p;

        for (p = node; p != NULL; p = p->parent)
          p->branch_size++;
      }

      /* Rebalance.  */
      if (node->left == NULL && node->parent != NULL)
        rebalance (list, node, 1, node->parent);
    }

#if WITH_HASHTABLE
  /* Add node to the hash table.
     Note that this is only possible _after_ the node has been added to the
     tree structure, because add_to_bucket() uses node_position().  */
  if (add_to_bucket (list, new_node) < 0)
    {
      gl_tree_remove_node_from_tree (list, new_node);
      free (new_node);
      return NULL;
    }
  hash_resize_after_add (list);
#endif

  return new_node;
}

static gl_list_node_t
gl_tree_nx_add_before (gl_list_t list, gl_list_node_t node, const void *elt)
{
  /* Create new node.  */
  gl_list_node_t new_node;
  bool height_inc;

  new_node =
    (struct gl_list_node_impl *) malloc (sizeof (struct gl_list_node_impl));
  if (new_node == NULL)
    return NULL;

  new_node->left = NULL;
  new_node->right = NULL;
  new_node->balance = 0;
  new_node->branch_size = 1;
  new_node->value = elt;
#if WITH_HASHTABLE
  new_node->h.hashcode =
    (list->base.hashcode_fn != NULL
     ? list->base.hashcode_fn (new_node->value)
     : (size_t)(uintptr_t) new_node->value);
#endif

  /* Add it to the tree.  */
  if (node->left == NULL)
    {
      node->left = new_node;
      node->balance--;
      height_inc = (node->right == NULL);
    }
  else
    {
      for (node = node->left; node->right != NULL; )
        node = node->right;
      node->right = new_node;
      node->balance++;
      height_inc = (node->left == NULL);
    }
  new_node->parent = node;

  /* Update branch_size fields of the parent nodes.  */
  {
    gl_list_node_t p;

    for (p = node; p != NULL; p = p->parent)
      p->branch_size++;
  }

  /* Rebalance.  */
  if (height_inc && node->parent != NULL)
    rebalance (list, node, 1, node->parent);

#if WITH_HASHTABLE
  /* Add node to the hash table.
     Note that this is only possible _after_ the node has been added to the
     tree structure, because add_to_bucket() uses node_position().  */
  if (add_to_bucket (list, new_node) < 0)
    {
      gl_tree_remove_node_from_tree (list, new_node);
      free (new_node);
      return NULL;
    }
  hash_resize_after_add (list);
#endif

  return new_node;
}

static gl_list_node_t
gl_tree_nx_add_after (gl_list_t list, gl_list_node_t node, const void *elt)
{
  /* Create new node.  */
  gl_list_node_t new_node;
  bool height_inc;

  new_node =
    (struct gl_list_node_impl *) malloc (sizeof (struct gl_list_node_impl));
  if (new_node == NULL)
    return NULL;

  new_node->left = NULL;
  new_node->right = NULL;
  new_node->balance = 0;
  new_node->branch_size = 1;
  new_node->value = elt;
#if WITH_HASHTABLE
  new_node->h.hashcode =
    (list->base.hashcode_fn != NULL
     ? list->base.hashcode_fn (new_node->value)
     : (size_t)(uintptr_t) new_node->value);
#endif

  /* Add it to the tree.  */
  if (node->right == NULL)
    {
      node->right = new_node;
      node->balance++;
      height_inc = (node->left == NULL);
    }
  else
    {
      for (node = node->right; node->left != NULL; )
        node = node->left;
      node->left = new_node;
      node->balance--;
      height_inc = (node->right == NULL);
    }
  new_node->parent = node;

  /* Update branch_size fields of the parent nodes.  */
  {
    gl_list_node_t p;

    for (p = node; p != NULL; p = p->parent)
      p->branch_size++;
  }

  /* Rebalance.  */
  if (height_inc && node->parent != NULL)
    rebalance (list, node, 1, node->parent);

#if WITH_HASHTABLE
  /* Add node to the hash table.
     Note that this is only possible _after_ the node has been added to the
     tree structure, because add_to_bucket() uses node_position().  */
  if (add_to_bucket (list, new_node) < 0)
    {
      gl_tree_remove_node_from_tree (list, new_node);
      free (new_node);
      return NULL;
    }
  hash_resize_after_add (list);
#endif

  return new_node;
}
