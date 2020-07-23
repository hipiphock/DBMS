package main

const RED = false
const BLACK = true

type TreeNode struct {
	parent TreeNode
	left   TreeNode
	right  TreeNode

	color bool
	key   int8
}

// TODO:
// Insertion
// Deletion
// Balancing
