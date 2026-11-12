import { Callout } from 'fumadocs-ui/components/callout';
import defaultMdxComponents from 'fumadocs-ui/mdx';
import type { MDXComponents } from 'mdx/types';
import type { ComponentProps } from 'react';
import { cn } from '@/lib/cn';

// A callout is a card washed in its own colour: a faint tint, a border of the same hue, its title in it, and its inline
// code tinted to match. It reads at the page's own size, the icon centred on the title's line. Fumadocs draws it as a
// bar (role="none"), an icon, and a column of a title <p> and a text <div>; the bar gives way to the border.
function Notice({ className, ...props }: ComponentProps<typeof Callout>) {
  return (
    <Callout
      className={cn(
        'gap-3 rounded-xl border-(--callout-color)/30 bg-(--callout-color)/[0.07] p-4 text-[0.9375rem]',
        '[&>[role=none]]:hidden [&>svg]:mt-0.5 [&_p]:leading-[1.6] [&>div:last-child]:gap-1',
        '[&>div:last-child>p]:text-(--callout-color) [&>div:last-child>div]:text-fd-foreground/85',
        '[&_:not(pre)>code]:border-(--callout-color)/25 [&_:not(pre)>code]:bg-(--callout-color)/10',
        className,
      )}
      {...props}
    />
  );
}

export function getMDXComponents(components?: MDXComponents) {
  return {
    ...defaultMdxComponents,
    Callout: Notice,
    ...components,
  } satisfies MDXComponents;
}

export const useMDXComponents = getMDXComponents;

declare global {
  type MDXProvidedComponents = ReturnType<typeof getMDXComponents>;
}
