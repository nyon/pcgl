module PCGL
  module Statement
    # Base class for all statements in pcgl
    class Base
      attr_accessor :line
      attr_accessor :indentation
      attr_accessor :children
      attr_accessor :meta

      def initialize(line = '', meta = {})
        @line        = line
        @children    = []
        @indentation = 0
        @meta        = meta
      end

      def add_child(child)
        @children.push(child)
      end

      def level
        @indentation / 4
      end

      def self.descendants
        descendants = []
        ObjectSpace.each_object(singleton_class) do |k|
          descendants.unshift k unless k == self
        end
        descendants
      end

      def self.priority
        raise "Please implement priority in subclass #{name}"
      end

      def self.can_parse?(_line)
        raise "Please implement can_parse? in subclass #{name}"
      end

      def process
        @indentation = Base.indent_level(@line)
        self
      end

      def statement_name
        @statement_name ||= self.class.name.demodulize.to_snakecase
      end

      def accept(visitor)
        visitor.try("pre_visit_#{statement_name}".to_sym, self)

        visitor.try("visit_#{statement_name}".to_sym, self)
        transform = visitor.try("transform_#{statement_name}".to_sym, self)
        @children.map! do |child|
          child.accept(visitor) || child
        end
        visitor.try("post_visit_#{statement_name}".to_sym, self)
        transform
      end

      def self.indent_level(line)
        indentation = 0
        line.chars.each do |char|
          break if char != "\t" && char != ' '
          indentation += if char == "\t"
                           4
                         else
                           1
                         end
        end
        indentation
      end

      def to_s
        self.class.name.to_s + ' ' + @meta.to_s
      end
    end
  end
end
