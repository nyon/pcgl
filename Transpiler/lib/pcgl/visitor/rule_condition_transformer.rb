module PCGL
  module Visitor
    # combines several rules into one single rule with
    # conditions
    class RuleConditionTransformer < Base
      attr_accessor :rules
      attr_accessor :rules_iterator

      def initialize(rules)
        @rules = rules
        @rules_iterator = {}
        @rules.each do |key, _|
          @rules_iterator[key] = 0
        end
      end

      def visit_rule_definition(object)
        return unless one_unconditional_child?(object.children)
        object.children = object.children.first.children
      end

      def one_unconditional_child?(children)
        children.length == 1 && !children.first.meta[:condition]
      end

      def transform_rule(object)
        name = object.meta[:name]
        object.meta[:condition] = apply_syntactic_sugar(object.meta[:condition])

        new_class = Statement::ElseIf

        if (@rules_iterator[name]).zero?
          new_class = Statement::If
        elsif object.meta[:condition].nil?
          new_class = Statement::Else
        end

        @rules_iterator[name] += 1
        clone_object_without_name(new_class, object)
      end

      def clone_object_without_name(clazz, object)
        ret = clazz.new('')
        ret.meta = object.meta
        ret.meta.delete(:name)
        ret.children = object.children
        ret
      end
    end
  end
end
