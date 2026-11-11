import { DocsLayout } from 'fumadocs-ui/layouts/glass'
import { baseOptions } from '@/lib/layout.shared'
import { referenceTree } from '@/lib/reference'
import { source } from '@/lib/source'

const docs = source.getPageTree()
const tree = { ...docs, children: [...docs.children, referenceTree()] }

export default function Layout({ children }: LayoutProps<'/'>) {
  return (
    <DocsLayout tree={tree} {...baseOptions()}>
      {children}
    </DocsLayout>
  )
}
