type Props = { html?: string; code: string }

export function Snippet({ html, code }: Props) {
  return html ? (
    <div className="signature" dangerouslySetInnerHTML={{ __html: html }} />
  ) : (
    <pre className="signature">
      <code>{code}</code>
    </pre>
  )
}
