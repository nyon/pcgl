module PCGL
  module Visitor
    # base class for implementing the visitor pattern
    class Base
      def initialize
      end

      def predefined_functions
        %w(random_surface_point random_float random_vertex random_vector random_point)
      end

      def apply_syntactic_sugar(code)
        return if code.nil?
        # necessary transformations
        transform_predefined_functions!(code)

        # some optimizations
        code.gsub!(/([^ \^]+)\^2/, '(\1*\1)')
        code.gsub!(/([^ \^]+)\^3/, '(\1*\1*\1)')

        code.gsub!(/\bPI\b/, '3.141592653589793238f')

        # syntactic sugar
        transform_shortcuts!(code)
        code
      end

      private

      def transform_shortcuts!(code)
        code.gsub!(/([^ \^]+)\^([^ \^])/, 'pow(\1,\2)')
        code.gsub!('width', 'fabsf(obb->r.x)')
        code.gsub!('height', 'fabsf(obb->r.y)')
        code.gsub!('depth', 'fabsf(obb->r.z)')
        code.gsub!(/([0-9]+(\.[0-9]*)?)%/) do |match|
          match.chomp('%').to_f / 100.0
        end
        code.gsub!(/([0-9]+(\.[0-9]*)?)°/) do |match|
          match.chomp('°').to_f * 0.017453293
        end
      end

      def transform_predefined_functions!(code)
        predefined_functions.each do |function|
          code.gsub!(function, "#{function}()")
        end
      end
    end
  end
end
