#include <iostream>
#include <cassert>

#include "ast.h"

std::ostream& operator<<(std::ostream& stream, const Node& node) {
    switch (node.type) {
        case NodeType::None:
            assert(false);
            break;
        case NodeType::Number: {
            const NumberNode& number_node = static_cast<const NumberNode&>(node);
            stream << number_node.number_token;
            break;
        }
        case NodeType::BinOp: {
            const BinOpNode& bin_op_node = static_cast<const BinOpNode&>(node);
            stream << "(" << *bin_op_node.left << ", " << bin_op_node.bin_op_token << ", " << *bin_op_node.right << ")";
            break;
        }
    }

    return stream;
}
