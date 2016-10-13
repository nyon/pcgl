module PCGL
  module Statement
    # extends all statements with debug functionalities
    class Base
      def debug(depth = '')
        puts depth + to_s
        @children.each do |child|
          child.debug(depth + '..')
        end
      end
    end
  end
end
